#ifndef STRING_UTIL_H
#define STRING_UTIL_H
#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include <functional>
#include <algorithm>
#include <regex>
#include "../typedefs.h"

namespace util {
	class string_util {
	public:
		using group = std::vector<std::string>;
		using group_callback = std::function<void(group &)>;
	public:
		// C++ implementation of C#'s String.Split, splits input 'str' into substrings returned in a vector
		static inline std::optional<group> split_by_delim(std::string str, const std::string &delim) {
			auto pos = str.find(delim);
			
			// no delim matches
			if (pos == std::string::npos)
				return std::nullopt;

			group ret{};
			do {
				auto match = str.substr(0, pos);
				ret.emplace_back(match);
				str.erase(0, pos + delim.size());
			} while ((pos = str.find(delim)) != std::string::npos);
			ret.emplace_back(str);

			return { ret };
		} 

		static inline std::string prompt_input(const std::string &prompt = "") {
			if (!prompt.empty())
				std::cout << prompt << std::endl;

			std::string temp;
			std::getline(std::cin, temp);
			return std::move(temp);
		}

		template<typename T>
		static void append_with_delim(std::string &str, const T &value, const char *delim) {
			if (std::is_same<T, std::string>::value)
				str.append(value).append(delim);
			else if (std::is_same<T, std::wstring>::value)
				printf("string_util::append_with_delim -> string types not compatible!\n");
			else
				str.append(std::to_string(value)).append(delim);
		}

		static bool is_number_string(const std::string &str) {
			return std::all_of(str.begin(), str.end(), [](char c) { return isdigit(c); });
		}

		static bool is_email_string(const std::string &str) {
			static const std::regex email_regex("^[_a-z0-9-]+(.[_a-z0-9-]+)*@[a-z0-9-]+(.[a-z0-9-]+)*(.[a-z]{2,4})$");
			return std::regex_match(str, email_regex);
		}
	};
} // namespace util

#endif STRING_UTIL_H // STRING_UTIL_H