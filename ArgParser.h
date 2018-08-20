#pragma once

#include <string>
#include <unordered_map>
#include <charconv>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <fmt/format.h>
class ArgParser {
private:
	static inline std::string trim(std::string s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));

		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());

		return s;
	}

public:
	ArgParser() = default;
	ArgParser(const std::string& description);
	
	ArgParser(const ArgParser&) = delete;
	ArgParser(ArgParser&&) = delete;


	~ArgParser() = default;

	void parseArguments(int argc, char **argv)  noexcept;

	void addArg(const std::string& argKey,
		const std::string& description,
		bool required)  noexcept;

	template<class _Type = std::string>
	_Type get(const std::string& argKey) const {

		std::string arg(this->trim(argKey));

		if (!this->isSet(arg)) {
			throw std::runtime_error(fmt::format("Argument '{}' is not set", arg));
		}
		if constexpr (std::is_same<_Type, char>::value) {
			return m_args.at(arg)[0];
		}else if constexpr (std::is_arithmetic<_Type>::value) {
			_Type result;
			auto data = m_args.at(arg);

			auto cvt = std::from_chars(data.c_str(),
				data.c_str() + data.size(), result);

			if (cvt.ec == std::errc::invalid_argument) {
				throw std::runtime_error(fmt::format("Could not convert '{}' from std::string to arithmetic type", data));
			}

			return result;
		}else if constexpr (std::is_same<_Type, std::string>::value) {
			return m_args.at(arg);
		} else  {
			throw std::runtime_error("ArgParser::get<T> - Invalid data type");
		}
	}

	void help() const noexcept {
		std::cout << "\n" << m_description << "\n";

		std::cout << "\t--help: Display this message\n\n";

		if (m_required_args.size() > 0) {
			std::cout << "[::] Required arguments\n";
			for (const auto& argument : m_required_args) {
				fmt::print("\t --{}: {}\n\n", argument.first, argument.second);
			}
		}


		if (m_opt_args.size() > 0) {
			std::cout << "[::] Optional arguments\n";

			for (const auto& argument : m_opt_args) {
				fmt::print("\t --{}: {}\n\n", argument.first, argument.second);
			}

		}
		exit(EXIT_SUCCESS);
	}

	//check if a argument is set
	bool isSet(const std::string& argKey) const {
		return m_args.find(argKey) != m_args.end();
	}


private:
	std::unordered_map<std::string, std::string> m_required_args,
		m_opt_args,
		m_args;

	std::string m_description;
};

