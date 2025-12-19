#include <iostream>
#include <boost/program_options.hpp>
#include <cppdb/frontend.h>
#include "command_arguments.hpp"
#include "pages/pages.hpp"
#include "common/common.hpp"

#ifndef VERSIONNUMBER
#define VERSIONNUMBER "0.1.0"
#endif

CommandArguments cmdargs;

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


void write_files(cppdb::session& database) {
	PrintStartEndTimer t("write_files");
	write_html_index(database, cmdargs.output_dir);
	// Generate map pages
	std::vector<std::string> map_list = get_most_played_maps(database);
	for (const std::string& mapname : map_list) {
		write_html_map(database, mapname, cmdargs.output_dir);
	}
	// Generate recent game pages (called from index generation now, but ensuring all are written)
	std::vector<OastatGame> games;
	getRecentGames(database, games);
	for (const OastatGame& game : games) {
		write_html_game(database, game, cmdargs.output_dir);
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
