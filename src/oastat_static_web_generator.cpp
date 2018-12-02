#include <iostream>
#include <boost/program_options.hpp>
#include <cppdb/frontend.h>
#include "command_arguments.hpp"
#include <fstream>
#include <ctemplate/template.h>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip> 

#ifndef VERSIONNUMBER
#define VERSIONNUMBER "0.1.0"
#endif

CommandArguments cmdargs;

const char* const SAGO_CONNECTION_STRING = "SAGO_CONNECTION_STRING";

std::string timestamp_now_as_string(cppdb::session& database) {
	cppdb::statement st = database.prepare("SELECT now()");
	cppdb::result res = st.query();
	if(res.next()) {
		std::string value;
		res >> value;
		return value;
	}
	return "";
}

static void create_dir_recursive(const std::string& path) {
	std::string command = std::string("mkdir -p \"")+path+"\"";
	int ret = std::system(command.c_str());
	if (ret != 0) {
		std::cerr << "Failed to create: " << path << "\n";
	}
}

static void copy_static_files(const std::string& source, const std::string& destination) {
	std::string command = "cp -r \""+source+"\"/* \""+destination+"/\"";
	int ret = std::system(command.c_str());
	if (ret != 0) {
		std::cerr << "copy with: " << command << "\n";
	}
}

std::string ZeroPadNumber(int num, int size = 2)
{
	std::ostringstream ss;
	ss.clear();
	ss << std::setw( size ) << std::setfill( '0' ) << num;
	return ss.str();
}

std::string getTimeStamp(const tm& _datetime)
{
	std::string s = ZeroPadNumber(_datetime.tm_year+1900,4) + "-" + ZeroPadNumber(_datetime.tm_mon+1) + "-"
		+ ZeroPadNumber(_datetime.tm_mday) + " " + ZeroPadNumber(_datetime.tm_hour) + ":"
		+ ZeroPadNumber(_datetime.tm_min) + ":" + ZeroPadNumber(_datetime.tm_sec);
	return s;
}

static std::map<int, int> kills_by_player;
static std::map<int, int> player_deaths;

void populate_player_deaths(cppdb::session& database, const std::vector<int>& player_ids) {
	std::string sql = "select count(0) from oastat.oastat_kills k where k.attacker <> k.target and target = ?";
	cppdb::statement st = database.prepare(sql);
	for (int player_id : player_ids) {
		st.bind(1, player_id);
		int deaths;
		cppdb::result res = st.query();
		if (res.next()) {
			res >> deaths;
			player_deaths[player_id] = deaths;
		}
	}
}

std::vector<int> get_top_killers(cppdb::session& database, int count) {
	std::vector<int> ret;
	std::string sql = "select p.playerid, count(0) c from oastat.oastat_players p, oastat.oastat_kills k where p.playerid = k.attacker and k.attacker <> k.target and p.playerid <> 0 group by p.playerid order by c desc limit ?";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, count);
	cppdb::result res = st.query();
	while(res.next()) {
		int player_id;
		int kills;
		res >> player_id >> kills;
		kills_by_player[player_id] = kills;
		ret.push_back(player_id);
	}
	return ret;
}


struct OastatPlayer {
	int playerid = 0;
	std::string nickname;
	tm lastseen;
	std::string isBot = "n";
	std::string model;
	std::string headmodel;
};

static std::map<int, OastatPlayer> players;

OastatPlayer getPlayer(cppdb::session& database, int playerid) {
	OastatPlayer ret;
	if (players.count(playerid)) {
		ret = players[playerid];
	}
	cppdb::statement st = database.prepare("select playerid, lastseen, isbot, model, headmodel, nickname from oastat.oastat_players where playerid = ?");
	st.bind(1, playerid);
	cppdb::result res = st.query();
	if (res.next()) {
		res >> ret.playerid >> ret.lastseen >> ret.isBot >> ret.model >> ret.headmodel >> ret.nickname;
		players[playerid] = ret;
	}
	return ret;
}

void write_html_index(cppdb::session& database) {
	ctemplate::TemplateDictionary index_tpl("templates/index.tpl");
	index_tpl.SetValue("GENERATION_DATE", timestamp_now_as_string(database));
	std::vector<int> player_list = get_top_killers(database, 25);
	populate_player_deaths(database, player_list);
	for (int i = 0; i < player_list.size(); ++i) {
		OastatPlayer p = getPlayer(database, player_list.at(i));
		ctemplate::TemplateDictionary* sub_dict = index_tpl.AddSectionDictionary("PLAYER_LIST");
		sub_dict->SetValue("EVEN_LINE", (i%2)?"1":"0");
		sub_dict->SetValue("PLAYER_NAME", p.nickname);
		sub_dict->SetValue("PLAYER_LAST_SEEN", getTimeStamp(p.lastseen));
		sub_dict->SetValue("PLAYER_KILLS", std::to_string(kills_by_player[p.playerid]));
		sub_dict->SetValue("PLAYER_DEATHS", std::to_string(player_deaths[p.playerid]));
		sub_dict->SetValue("PLAYER_IS_BOT", p.isBot);
	}
	std::string output;
	ctemplate::ExpandTemplate("templates/index.tpl", ctemplate::DO_NOT_STRIP, &index_tpl, &output);
	std::ofstream myfile;
	myfile.open (cmdargs.output_dir+"/index.html");
	myfile << output;
	myfile.close();
}

void write_files(cppdb::session& database) {
	write_html_index(database);
}




void do_dbtest(cppdb::session& database) {
	cppdb::statement st = database.prepare("SELECT now()");
	cppdb::result res = st.query();
	if(res.next()) {
		std::string value;
		res >> value;
		std::cout << value << "\n";
	}
}

int main(int argc, const char* argv[]) {
	boost::program_options::options_description desc("Options");
	desc.add_options()
	("version", "Print version information and quit")
	("help,h", "Print basic usage information to stdout and quit")
	("output-dir", boost::program_options::value<std::string>(), "The directory to write the output to.")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 0;
	}
	if (vm.count("version")) {
		std::cout << "oastat_static_web_generator " << VERSIONNUMBER << "\n";
		return 0;
	}
	if (vm.count("output-dir")) {
		cmdargs.output_dir = vm["output-dir"].as<std::string>();
	}
	const char* connectstring_env = getenv(SAGO_CONNECTION_STRING);
	if (connectstring_env) {
		cmdargs.connectstring = connectstring_env;
	}
	cppdb::session database(cmdargs.connectstring);
	do_dbtest(database);
	create_dir_recursive(cmdargs.output_dir+"/static");
	copy_static_files("static_content", cmdargs.output_dir+"/static");
	write_files(database);
	return 0;
}
