#include "stdafx.h"
#include "ArgParser.h"

ArgParser::ArgParser(const std::string & description) : m_description(description){ }

void ArgParser::parseArguments(int argc, char **argv) noexcept {

	if (argc == 1) {
		std::cout << "Missing arguments\n";
		this->help();
	} else if (argc == 2 && strcmp(argv[1], "--help") == 0) {
		this->help();
	}

	for (int i = 1; i < argc; i += 2) {
		auto arg = std::string(argv[i]).substr(2,-1);
		

		if (m_required_args.find(arg) != m_required_args.end() ||
			m_opt_args.find(arg) != m_opt_args.end()) {
			
			if (argc > i + 1) {
				m_args.insert({ arg, argv[i + 1]});
			} else {
				fmt::print("Missing data do argument '{}'", arg);
				this->help();
			}
		
		} else {
			fmt::print("Invalid argument '{}'", arg);
			this->help();
		}
	}

	for (const auto& argument : m_required_args) {
		if (m_args.find(argument.first) == m_args.end()) {
			fmt::print("Missing required argument '{}'", argument.first);
			this->help();
		}
	}

}

void ArgParser::addArg(const std::string& argKey,
						const std::string& description, bool required)  noexcept {
	
	std::string arg(this->trim(argKey));

	if (!arg.empty()) {
		if (required) {
			m_required_args.insert({ arg, description });
		} else {
			m_opt_args.insert({ arg, description });
		}
	}
}

