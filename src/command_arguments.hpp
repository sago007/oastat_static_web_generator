

#ifndef COMMAND_ARGUMENTS_HPP
#define COMMAND_ARGUMENTS_HPP

struct CommandArguments {
	std::string connectstring = "mysql:database=testschema;user=testuser;password=password";
	std::string output_dir = "output";
};

#endif /* COMMAND_ARGUMENTS_HPP */

