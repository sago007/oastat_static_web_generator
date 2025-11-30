#include <iostream>
#include <boost/program_options.hpp>
#include <cppdb/frontend.h>
#include "command_arguments.hpp"
#include <fstream>
#include <ctemplate/template.h>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iomanip>
#include "common/common.hpp"

#ifndef VERSIONNUMBER
#define VERSIONNUMBER "0.1.0"
#endif

CommandArguments cmdargs;

const size_t MAX_MAP_LIST = 10;

const char* const SAGO_CONNECTION_STRING = "SAGO_CONNECTION_STRING";


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

class PrintStartEndTimer {
	std::string name;
	public:
	explicit PrintStartEndTimer(const std::string& name) {
		this->name = name;
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);
		std::cerr << "start_" << name << ": " << getTimeStamp(*now) << "\n";
	}

	~PrintStartEndTimer() {
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);
		std::cerr << "end_" << name << ": " << getTimeStamp(*now) << "\n";
	}

};

static std::map<int, int> kills_by_player;
static std::map<int, int> player_deaths;
static std::map<int, std::map<int, int>> player_awards;
static std::map<int, std::map<int, int>> player_weapon_kills;
static std::map<std::string, MapInfo> map_infos;
static std::set<int> generated_games;
static std::set<std::string> generated_maps;

void populate_player_deaths(cppdb::session& database, const std::vector<int>& player_ids) {
	PrintStartEndTimer t("populate_player_deaths");
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

void populate_player_awards(cppdb::session& database, const std::vector<int>& player_ids) {
	PrintStartEndTimer t("populate_player_awards");
	std::string sql = "select award, count(0) c from oastat.oastat_awards where player = ? group by  player, award";
	cppdb::statement st = database.prepare(sql);
	for (int player_id : player_ids) {
		st.bind(1, player_id);
		int award;
		int count;
		cppdb::result res = st.query();
		while (res.next()) {
			res >> award >> count;
			player_awards[player_id][award] = count;
		}
	}
}

void populate_player_kills(cppdb::session& database, const std::vector<int>& player_ids) {
	PrintStartEndTimer t("populate_player_kills");
	std::string sql = "select modtype, count(0) c from oastat.oastat_kills where attacker <> target and attacker = ? group by modtype";
	cppdb::statement st = database.prepare(sql);
	for (int player_id : player_ids) {
		st.bind(1, player_id);
		int weapon;
		int count;
		cppdb::result res = st.query();
		while (res.next()) {
			res >> weapon >> count;
			player_weapon_kills[player_id][weapon] = count;
		}
	}
}

std::vector<int> get_top_killers(cppdb::session& database, int count) {
	PrintStartEndTimer t("get_top_killers");
	std::vector<int> ret;
	std::string sql = "select p.playerid, p.isbot b, count(0) c from oastat.oastat_players p, oastat.oastat_kills k where p.playerid = k.attacker and k.attacker <> k.target and p.playerid <> 0 group by p.playerid, p.isbot order by b, c desc limit ?";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, count);
	cppdb::result res = st.query();
	while(res.next()) {
		int player_id;
		int kills;
		std::string isBot;
		res >> player_id >> isBot >> kills;
		kills_by_player[player_id] = kills;
		ret.push_back(player_id);
	}
	return ret;
}

std::vector<std::string> get_most_played_maps(cppdb::session& database) {
	PrintStartEndTimer t("get_most_played_maps");
	std::vector<std::string> ret;
	std::string sql = "select mapname, count(0) c, max(time) last_game from oastat.oastat_games group by mapname order by c desc";
	cppdb::statement st = database.prepare(sql);
	cppdb::result res = st.query();
	while(res.next()) {
		std::string mapname;
		MapInfo info;
		res >> mapname >> info.times_played >> info.last_played;
		map_infos[mapname] = info;
		ret.push_back(mapname);
	}
	return ret;
}


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

struct OastatGame {
	int gamenumber = 0;
	int gametype = 0;
	std::string mapname;
	tm time = {};
	std::string basegame;
	int second = 0;
	std::string servername;
};

struct ScorePoint {
	int playerid = 0;
	int second = 0;
	int score = 0;
};


void write_html_game(cppdb::session& database, const OastatGame& game);

void getRecentGames(cppdb::session& database, std::vector<OastatGame>& games) {
	PrintStartEndTimer t("getRecentGames");
	std::string sql = "select gamenumber, gametype, mapname, time, basegame, second, servername from oastat.oastat_games order by gamenumber desc limit 10";
	cppdb::statement st = database.prepare(sql);
	cppdb::result res = st.query();
	while(res.next()) {
		OastatGame game;
		res >> game.gamenumber >> game.gametype >> game.mapname >> game.time >> game.basegame >> game.second >> game.servername;
		write_html_game(database, game);
		games.push_back(game);
	}
}

void getGameScoreTotal(cppdb::session& database, int gamenumber, std::vector<std::pair<int,int>>& scores) {
	std::string sql = "select player, count(0) c from oastat.oastat_points where gamenumber = ? group by player order by c desc";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, gamenumber);
	cppdb::result res = st.query();
	while(res.next()) {
		int playerid;
		int count;
		res >> playerid >> count;
		scores.push_back(std::pair<int,int>(playerid, count));
	}
}

void getGameScoreProgression(cppdb::session& database, int gamenumber, std::vector<ScorePoint>& progression) {
	std::string sql = "SELECT player, `second` as the_second, score FROM oastat.oastat_points where gamenumber = ? ORDER BY the_second, player";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, gamenumber);
	cppdb::result res = st.query();
	while(res.next()) {
		ScorePoint point;
		res >> point.playerid >> point.second >> point.score;
		progression.push_back(point);
	}
}

void addPlayerToTemplateDict(ctemplate::TemplateDictionary* dict, const OastatPlayer& player) {
	dict->SetValue("PLAYER_ID", std::to_string(player.playerid));
	dict->SetValue("PLAYER_HEADMODEL", player.headmodel);
	dict->SetValue("PLAYER_ISBOT", player.isBot);
	dict->SetValue("PLAYER_LASTSEEN", getTimeStamp(player.lastseen));
	dict->SetValue("PLAYER_MODEL", player.model);
	dict->SetValue("PLAYER_NICKNAME", player.nickname);
}

void write_html_game(cppdb::session& database, const OastatGame& game) {
	// Skip if already generated
	if (generated_games.count(game.gamenumber)) {
		return;
	}
	generated_games.insert(game.gamenumber);
	ctemplate::TemplateDictionary game_tpl("templates/game.tpl");
	game_tpl.SetValue("GENERATION_DATE", timestamp_now_as_string(database));
	game_tpl.SetValue("GAME_NUMBER", std::to_string(game.gamenumber));
	game_tpl.SetValue("GAME_MAP", game.mapname);
	game_tpl.SetValue("GAME_SERVERNAME", game.servername);
	
	std::vector<std::pair<int,int>> scores;
	getGameScoreTotal(database, game.gamenumber, scores);
	for (size_t i = 0; i < scores.size(); ++i) {
		ctemplate::TemplateDictionary* sub_dict = game_tpl.AddSectionDictionary("SCORES_LIST");
		const OastatPlayer& p = getPlayer(database, scores.at(i).first);
		sub_dict->SetValue("ID", std::to_string(scores.at(i).first));
		sub_dict->SetValue("SCORE", std::to_string(scores.at(i).second));
		addPlayerToTemplateDict(sub_dict, p);
	}
	
	// Get score progression for chart
	std::vector<ScorePoint> progression;
	getGameScoreProgression(database, game.gamenumber, progression);
	
	// Build JSON data for D3.js chart
	// Group by player to create series
	std::map<int, std::vector<ScorePoint>> playerSeries;
	for (const auto& point : progression) {
		playerSeries[point.playerid].push_back(point);
	}
	
	// Generate JSON for each player's score progression
	std::stringstream json_data;
	json_data << "[";
	bool first_player = true;
	for (const auto& series : playerSeries) {
		if (!first_player) json_data << ",";
		first_player = false;
		
		const OastatPlayer& p = getPlayer(database, series.first);
		json_data << "{\"player\":\"" << p.nickname << "\",\"playerid\":" << series.first << ",\"data\":[";
		
		bool first_point = true;
		for (const auto& point : series.second) {
			if (!first_point) json_data << ",";
			first_point = false;
			json_data << "{\"second\":" << point.second << ",\"score\":" << point.score << "}";
		}
		json_data << "]}";
	}
	json_data << "]";
	
	// Write JSON to separate file
	std::ofstream json_file;
	json_file.open(cmdargs.output_dir+"/game/"+std::to_string(game.gamenumber)+".json");
	json_file << json_data.str();
	json_file.close();
	
	std::string output;
	ctemplate::ExpandTemplate("templates/game.tpl", ctemplate::DO_NOT_STRIP, &game_tpl, &output);
	std::ofstream myfile;
	myfile.open (cmdargs.output_dir+"/game/"+std::to_string(game.gamenumber)+".html");
	myfile << output;
	myfile.close();
}

std::string getWeaponName(int modtype) {
	switch(modtype) {
		case MOD_SHOTGUN: return "Shotgun";
		case MOD_GAUNTLET: return "Gauntlet";
		case MOD_MACHINEGUN: return "Machinegun";
		case MOD_GRENADE:
		case MOD_GRENADE_SPLASH: return "Grenade";
		case MOD_ROCKET:
		case MOD_ROCKET_SPLASH: return "Rocket";
		case MOD_PLASMA:
		case MOD_PLASMA_SPLASH: return "Plasma";
		case MOD_RAILGUN: return "Railgun";
		case MOD_LIGHTNING: return "Lightning";
		case MOD_BFG:
		case MOD_BFG_SPLASH: return "BFG";
		case MOD_NAIL: return "Nailgun";
		case MOD_CHAINGUN: return "Chaingun";
		case MOD_TELEFRAG: return "Telefrag";
		case MOD_FALLING: return "Falling";
		default: return "Other";
	}
}

void getMapWeaponKills(cppdb::session& database, const std::string& mapname, std::map<int, int>& weapon_kills) {
	std::string sql = "select k.modtype, count(0) c from oastat.oastat_kills k, oastat.oastat_games g "
		"where g.gamenumber = k.gamenumber and g.mapname = ? and k.attacker <> k.target "
		"group by k.modtype order by c desc";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, mapname);
	cppdb::result res = st.query();
	while(res.next()) {
		int modtype;
		int count;
		res >> modtype >> count;
		weapon_kills[modtype] = count;
	}
}

void getMapRecentGames(cppdb::session& database, const std::string& mapname, std::vector<OastatGame>& games) {
	std::string sql = "select gamenumber, gametype, mapname, time, basegame, second, servername "
		"from oastat.oastat_games where mapname = ? order by gamenumber desc limit 10";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, mapname);
	cppdb::result res = st.query();
	while(res.next()) {
		OastatGame game;
		res >> game.gamenumber >> game.gametype >> game.mapname >> game.time >> game.basegame >> game.second >> game.servername;
		games.push_back(game);
		write_html_game(database, game);
	}
}

bool compareWeaponKillsDesc(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
	return a.second > b.second;
}

void write_html_map(cppdb::session& database, const std::string& mapname) {
	// Skip if already generated
	if (generated_maps.count(mapname)) {
		return;
	}
	generated_maps.insert(mapname);
	ctemplate::TemplateDictionary map_tpl("templates/map.tpl");
	map_tpl.SetValue("GENERATION_DATE", timestamp_now_as_string(database));
	map_tpl.SetValue("MAP_NAME", mapname);

	// Get weapon kills for this map
	std::map<int, int> weapon_kills;
	getMapWeaponKills(database, mapname, weapon_kills);

	// Combine splash damage with base weapon damage
	std::map<std::string, int> combined_weapon_kills;
	combined_weapon_kills["Shotgun"] = weapon_kills[MOD_SHOTGUN];
	combined_weapon_kills["Gauntlet"] = weapon_kills[MOD_GAUNTLET];
	combined_weapon_kills["Machinegun"] = weapon_kills[MOD_MACHINEGUN];
	combined_weapon_kills["Grenade"] = weapon_kills[MOD_GRENADE] + weapon_kills[MOD_GRENADE_SPLASH];
	combined_weapon_kills["Rocket"] = weapon_kills[MOD_ROCKET] + weapon_kills[MOD_ROCKET_SPLASH];
	combined_weapon_kills["Plasma"] = weapon_kills[MOD_PLASMA] + weapon_kills[MOD_PLASMA_SPLASH];
	combined_weapon_kills["Railgun"] = weapon_kills[MOD_RAILGUN];
	combined_weapon_kills["Lightning"] = weapon_kills[MOD_LIGHTNING];
	combined_weapon_kills["Nailgun"] = weapon_kills[MOD_NAIL];
	combined_weapon_kills["Chaingun"] = weapon_kills[MOD_CHAINGUN];
	combined_weapon_kills["BFG"] = weapon_kills[MOD_BFG] + weapon_kills[MOD_BFG_SPLASH];
	combined_weapon_kills["Telefrag"] = weapon_kills[MOD_TELEFRAG];
	combined_weapon_kills["Falling"] = weapon_kills[MOD_FALLING];

	// Sort weapons by kill count (descending)
	std::vector<std::pair<std::string, int>> sorted_weapons;
	for (const auto& wk : combined_weapon_kills) {
		if (wk.second > 0) {
			sorted_weapons.push_back(wk);
		}
	}
	std::stable_sort(sorted_weapons.begin(), sorted_weapons.end(), compareWeaponKillsDesc);

	for (const auto& wk : sorted_weapons) {
		ctemplate::TemplateDictionary* sub_dict = map_tpl.AddSectionDictionary("WEAPON_KILLS");
		sub_dict->SetValue("WEAPON_NAME", wk.first);
		sub_dict->SetValue("KILL_COUNT", std::to_string(wk.second));
	}

	// Get recent matches for this map
	std::vector<OastatGame> games;
	getMapRecentGames(database, mapname, games);
	for (size_t i = 0; i < games.size(); ++i) {
		const OastatGame& game = games[i];
		ctemplate::TemplateDictionary* sub_dict = map_tpl.AddSectionDictionary("RECENT_MATCHES");
		sub_dict->SetValue("GAMENUMBER", std::to_string(game.gamenumber));
		sub_dict->SetValue("TIME", getTimeStamp(game.time));
		sub_dict->SetValue("SERVERNAME", game.servername);
	}

	std::string output;
	ctemplate::ExpandTemplate("templates/map.tpl", ctemplate::DO_NOT_STRIP, &map_tpl, &output);
	std::ofstream myfile;
	myfile.open (cmdargs.output_dir+"/map/"+mapname+".html");
	myfile << output;
	myfile.close();
}

void write_html_index(cppdb::session& database) {
	ctemplate::TemplateDictionary index_tpl("templates/index.tpl");
	index_tpl.SetValue("GENERATION_DATE", timestamp_now_as_string(database));
	std::vector<int> player_list = get_top_killers(database, 25);
	populate_player_deaths(database, player_list);
	populate_player_awards(database, player_list);
	populate_player_kills(database, player_list);
	std::vector<OastatGame> games;
	getRecentGames(database, games);
	for (size_t i = 0; i < player_list.size(); ++i) {
		OastatPlayer p = getPlayer(database, player_list.at(i));
		ctemplate::TemplateDictionary* sub_dict = index_tpl.AddSectionDictionary("PLAYER_LIST");
		sub_dict->SetValue("EVEN_LINE", (i%2)?"1":"0");
		sub_dict->SetValue("PLAYER_NAME", p.nickname);
		sub_dict->SetValue("PLAYER_LAST_SEEN", getTimeStamp(p.lastseen));
		sub_dict->SetValue("PLAYER_KILLS", std::to_string(kills_by_player[p.playerid]));
		sub_dict->SetValue("PLAYER_DEATHS", std::to_string(player_deaths[p.playerid]));
		sub_dict->SetValue("PLAYER_IS_BOT", p.isBot);
		sub_dict->SetValue("AWARD_GAUNTLET", std::to_string(player_awards[p.playerid][AWARD_GAUNTLET]));
		sub_dict->SetValue("AWARD_IMPRESSIVE", std::to_string(player_awards[p.playerid][AWARD_IMPRESSIVE]));
		sub_dict->SetValue("AWARD_EXCELLENT", std::to_string(player_awards[p.playerid][AWARD_EXCELLENT]));
		sub_dict->SetValue("AWARD_CAPTURE", std::to_string(player_awards[p.playerid][AWARD_CAPTURE]));
		sub_dict->SetValue("AWARD_DEFENCE", std::to_string(player_awards[p.playerid][AWARD_DEFENCE]));
		sub_dict->SetValue("AWARD_ASSIST", std::to_string(player_awards[p.playerid][AWARD_ASSIST]));
	}
	for (size_t i = 0; i < player_list.size(); ++i) {
		OastatPlayer p = getPlayer(database, player_list.at(i));
		ctemplate::TemplateDictionary* sub_dict = index_tpl.AddSectionDictionary("PLAYER_WEAPON_LIST");
		sub_dict->SetValue("EVEN_LINE", (i%2)?"1":"0");
		sub_dict->SetValue("PLAYER_NAME", p.nickname);
		sub_dict->SetValue("WEAPON_SHOTGUN", std::to_string(player_weapon_kills[p.playerid][MOD_SHOTGUN]));
		sub_dict->SetValue("WEAPON_GAUNTLET", std::to_string(player_weapon_kills[p.playerid][MOD_GAUNTLET]));
		sub_dict->SetValue("WEAPON_MACHINEGUN", std::to_string(player_weapon_kills[p.playerid][MOD_MACHINEGUN]));
		sub_dict->SetValue("WEAPON_GRENADE", std::to_string(player_weapon_kills[p.playerid][MOD_GRENADE]+player_weapon_kills[p.playerid][MOD_GRENADE_SPLASH]));
		sub_dict->SetValue("WEAPON_ROCKET", std::to_string(player_weapon_kills[p.playerid][MOD_ROCKET]+player_weapon_kills[p.playerid][MOD_ROCKET_SPLASH]));
		sub_dict->SetValue("WEAPON_PLASMA", std::to_string(player_weapon_kills[p.playerid][MOD_PLASMA]+player_weapon_kills[p.playerid][MOD_PLASMA_SPLASH]));
		sub_dict->SetValue("WEAPON_RAILGUN", std::to_string(player_weapon_kills[p.playerid][MOD_RAILGUN]));
		sub_dict->SetValue("WEAPON_LIGHTNING", std::to_string(player_weapon_kills[p.playerid][MOD_LIGHTNING]));
		sub_dict->SetValue("WEAPON_NAILGUN", std::to_string(player_weapon_kills[p.playerid][MOD_NAIL]));
		sub_dict->SetValue("WEAPON_CHAINGUN", std::to_string(player_weapon_kills[p.playerid][MOD_CHAINGUN]));
		sub_dict->SetValue("WEAPON_BFG", std::to_string(player_weapon_kills[p.playerid][MOD_BFG]+player_weapon_kills[p.playerid][MOD_BFG_SPLASH]));
		sub_dict->SetValue("WEAPON_TELEFRAG", std::to_string(player_weapon_kills[p.playerid][MOD_TELEFRAG]));
		sub_dict->SetValue("WEAPON_FALLING", std::to_string(player_weapon_kills[p.playerid][MOD_FALLING]));
	}
	std::vector<std::string> map_list = get_most_played_maps(database);
	for (size_t i = 0; i < map_list.size() && i < MAX_MAP_LIST; ++i) {
		const std::string& mapname = map_list[i];
		const MapInfo& info = map_infos[mapname];
		ctemplate::TemplateDictionary* sub_dict = index_tpl.AddSectionDictionary("MAP_LIST");
		sub_dict->SetValue("EVEN_LINE", (i%2)?"1":"0");
		sub_dict->SetValue("MAP_NAME", mapname);
		sub_dict->SetValue("TIMES_PLAYED", std::to_string(info.times_played));
		sub_dict->SetValue("LAST_PLAYED", getTimeStamp(info.last_played));
	}
	for (size_t i = 0; i < games.size(); ++i) {
		const OastatGame& game = games[i];
		ctemplate::TemplateDictionary* sub_dict = index_tpl.AddSectionDictionary("RECENT_GAMES");
		sub_dict->SetValue("EVEN_LINE", (i%2)?"1":"0");
		sub_dict->SetValue("GAMENUMBER", std::to_string(game.gamenumber) );
		sub_dict->SetValue("MAPNAME", game.mapname);
		sub_dict->SetValue("TIME", getTimeStamp(game.time));
		sub_dict->SetValue("SERVERNAME", game.servername);
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
	// Generate map pages
	std::vector<std::string> map_list = get_most_played_maps(database);
	for (const std::string& mapname : map_list) {
		write_html_map(database, mapname);
	}
}




void write_current_time(cppdb::session& database) {
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
		std::cout << "\n" << "Connection details are read from the environemtn variable \"" << SAGO_CONNECTION_STRING << "\". ";
		std::cout << "The string must be in CppDb format. \n";
		std::cout << "Like: \"export " << SAGO_CONNECTION_STRING << "=mysql:database=oastat;username=openarena\"\n";
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
	write_current_time(database);
	create_dir_recursive(cmdargs.output_dir+"/static");
	create_dir_recursive(cmdargs.output_dir+"/game");
	create_dir_recursive(cmdargs.output_dir+"/map");
	copy_static_files("static_content", cmdargs.output_dir+"/static");
	write_files(database);
	return 0;
}
