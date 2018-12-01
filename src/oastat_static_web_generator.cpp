#include <iostream>
#include <boost/program_options.hpp>
#include <cppdb/frontend.h>
#include "command_arguments.hpp"
#include <fstream>
#include <ctemplate/template.h>

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


void write_html_index(cppdb::session& database) {
	ctemplate::TemplateDictionary index_tpl("templates/index.tpl");
	index_tpl.SetValue("GENERATION_DATE", timestamp_now_as_string(database));
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
