#ifndef DATETIME_H
#define DATETIME_H
#include <ctime>
#include <iostream> 
#include <iomanip>
#include <sstream>
#include <regex>
#include "../typedefs.h"
#include "string_util.h"
#include "math.h"

/*
	Date/time structure
	Usage:	auto now = util::date_time::now();
			auto timepoint = util::date_time("1/1/2019", "01:40");

	Author: Alice L. Jones 2019
*/

namespace util {
	enum date_format : u8 {
		DDMMYYYY,
		MMDDYYYY
	};

	constexpr auto DATE_PRINT_FORMAT = util::date_format::DDMMYYYY;

	enum months : u8 {
		JANUARY = 1,
		FEBRUARY,
		MARCH,
		APRIL,
		MAY,
		JUNE,
		JULY,
		AUGUST,
		SEPTEMBER,
		OCTOBER,
		NOVEMBER,
		DECEMBER
	};

	inline u8 days_in_month(months month) {
		switch (month) {
			case JANUARY:
			case MARCH:
			case MAY:
			case JULY:
			case AUGUST:
			case OCTOBER:
			case DECEMBER:
				return u8(31);
				break;
			case APRIL:
			case JUNE:
			case SEPTEMBER:
			case NOVEMBER:
				return u8(30);
				break;
			case FEBRUARY:
				return u8(28);
				break;
			default:
				break;
		}

		return u8{};
	}

	// can't get regex to work
	inline bool is_valid_date_string(const std::string &str) {
		if (!std::all_of(str.begin(), str.end(), [](char c) {
			return isdigit(c) || c == '/';
		}))
			return false;

		if (str.size() != 10u)
			return false;

		auto segments = string_util::split_by_delim(str, "/");
		if (!segments.has_value())
			return false;

		auto segv = segments.value();
		if (segv.empty())
			return false;

		auto day = (u8)strtoul(DATE_PRINT_FORMAT == DDMMYYYY ? segv[0].data() : segv[1].data(), nullptr, 0);
		auto month = (u8)strtoul(DATE_PRINT_FORMAT == DDMMYYYY ? segv[1].data() : segv[0].data(), nullptr, 0);
		auto year = (u8)strtoul(segv[2].data(), nullptr, 0);

		if (month < 1 || month > 12)
			return false;

		if (day > days_in_month((months)month) || day < 0)
			return false;

		return true;
	}

	struct date_time {
		inline date_time() noexcept : date{}, time{} { }

		inline static date_time now() {
			date_time out;
			out.init_now();
			return std::move(out);
		}

		// date input should either by 'DD/MM/YYYY' or 'MM/DD/YYYY' depending on fmt
		// time should always be in military time format 00:00:00 - 23:59:59 (seconds are optional and will default to 00)
		inline date_time(const std::string &date, const std::string &time = "") {
			if (date.empty())
				return;

			if (!is_valid_date_string(date))
				return;

			// seconds optional
			static std::regex time_regex("^([0-1]?[0-9]|[2][0-3]):([0-5][0-9])(:[0-5][0-9])?$");
			if (!time.empty() && !std::regex_match(time, time_regex))
				return;

			if (auto dvalues = string_util::split_by_delim(date, "/")) {
				auto values = dvalues.value();
				try {
					auto i = (size_t)DATE_PRINT_FORMAT;
					this->date = date_info(
						(u8)strtoul(values[i].data(), nullptr, 0),
						(u8)strtoul(values[(!i)].data(), nullptr, 0),
						(u32)strtoul(values[2].data(), nullptr, 0)
					).clamped();
				} catch (const std::out_of_range &e) {
					std::cerr << e.what();
					std::cerr << "Size of split date vector: " << values.size() << "\nElements:\n";
					for (auto &elem : values)
						std::cerr << '\t' << elem << std::endl;
					return;
				}
			}

			if (!time.empty()) {
				if (auto tvalues = string_util::split_by_delim(time, ":")) {
					auto values = tvalues.value();
					try {
						this->time = time_info(
							(u8)strtoul(values[0].data(), nullptr, 0),
							(u8)strtoul(values[1].data(), nullptr, 0),
							values.size() > 2 ? (u8)strtoul(values[2].data(), nullptr, 0) : u8{}
						).clamped();
					}
					catch (const std::out_of_range &e) {
						std::cerr << e.what();
						std::cerr << "Size of split time vector: " << values.size() << "\nElements:\n";
						for (auto &elem : values)
							std::cerr << '\t' << elem << std::endl;
						return;
					}
				}
			}
		}

		inline date_time(const date_time &cpy) {
			memcpy((void *)this, std::addressof(cpy), sizeof(date_time));
		}

		inline date_time &operator=(const date_time &cpy) {
			memcpy((void *)this, std::addressof(cpy), sizeof(date_time));
			return *this;
		}

		inline bool operator==(const date_time &rhs) const {
			return date == rhs.date && time == rhs.time;
		}

		inline bool operator!=(const date_time &rhs) const {
			return !(*this == rhs);
		}

		inline bool operator<(const date_time &rhs) const {
			if (date < rhs.date)
				return true;
			else if (date == rhs.date)
				return time < rhs.time;
			else
				return false;
		}

		inline bool operator<=(const date_time &rhs) const {
			if (date < rhs.date)
				return true;
			else if (date == rhs.date)
				return time <= rhs.time;
			return true;
		}

		inline bool operator>(const date_time &rhs) const {
			if (date > rhs.date)
				return true;
			else if (date == rhs.date)
				return time > rhs.time;
			else
				return false;
		}

		inline bool operator>=(const date_time &rhs) const {
			if (date > rhs.date)
				return true;
			else if (date == rhs.date)
				return time >= rhs.time;
			else
				return false;
		}

		inline friend std::ostream &operator<<(std::ostream &out, const date_time &data);
		inline friend std::istream &operator>>(std::istream &in, date_time &target);

		inline void print() const {
			std::cout << *this << std::endl;
		}

		inline std::string to_string(bool include_time = false) {
			std::stringstream buf{};
			std::tm time_info;
			time_info.tm_mday = date.day;
			time_info.tm_mon = date.month - 1;
			time_info.tm_year = date.year - 1900;

			buf << std::put_time(&time_info, "%d/%m/%Y");

			if (include_time) {
				time_info.tm_hour = time.hour;
				time_info.tm_min = time.minute;
				time_info.tm_sec = time.second;

				buf << " " << std::put_time(&time_info, "%H:%M:%S");
			}

			return std::move(buf.str());
		}

		struct date_info {
			// access to clamped
			friend struct date_time;

			u8 day, month;
			u32 year;

			inline date_info() noexcept : day{}, month{}, year{} { }
			inline date_info(u8 d, u8 m, u32 y) : day{ d }, year{ y }, month{ m } { }

			inline bool operator==(const date_info &cmp) const {
				return day == cmp.day && month == cmp.month && year == cmp.year;
			}

			inline bool operator!=(const date_info &cmp) const {
				return !(*this == cmp);
			}

			inline bool operator>(const date_info &cmp) const {
				if (year > cmp.year)
					return true;
				else {
					if (month > cmp.month)
						return true;
					else if (month == cmp.month)
						return day > cmp.day;
					else
						return false;
				}
			}

			inline bool operator>=(const date_info &cmp) const {
				if (year > cmp.year) {
					return true;
				} else if (year == cmp.year) {
					if (month > cmp.month)
						return true;
					else if (month == cmp.month)
						return day >= cmp.day;
					else
						return false;
				} else {
					return false;
				}
			}

			inline bool operator<(const date_info &cmp) const {
				return !(*this >= cmp);
			}

			inline bool operator<=(const date_info &cmp) const {
				return !(*this > cmp);
			}

		private:
			inline date_info &clamped() {
				// clamp month first since thats easy
				Clamp(month, u8(1), u8(12));
				Clamp(day, u8(1), days_in_month((months)month));

				// TODO: clamp year maybe?
				return *this;
			}
		} date;

		struct time_info {
			friend struct date_time;

			u8 hour, minute, second;

			inline time_info() noexcept : hour{}, minute{}, second{} { }
			inline time_info(u8 h, u8 m, u8 s) : hour{ h }, minute{ m }, second{ s } { }

			inline bool operator==(const time_info &cmp) const {
				return hour == cmp.hour && minute == cmp.minute && second == cmp.second;
			}

			inline bool operator!=(const time_info &cmp) const {
				return !(*this == cmp);
			}

			inline bool operator>(const time_info &cmp) const {
				if (hour > cmp.hour)
					return true;
				else if (hour <= cmp.hour)
					return false;
				else {
					if (minute > cmp.minute)
						return true;
					else if (minute == cmp.minute && second > cmp.second)
						return true;
					else
						return false;
				}
			}

			inline bool operator>=(const time_info &cmp) const {
				if (hour >= cmp.hour)
					return true;
				else if (hour < cmp.hour)
					return false;
				else {
					if (minute >= cmp.minute)
						return true;
					else if (minute == cmp.minute && second >= cmp.second)
						return true;
					else
						return false;
				}
			}

			inline bool operator<(const time_info &cmp) const {
				return !(*this >= cmp);
			}

			inline bool operator<=(const time_info &cmp) const {
				return !(*this > cmp);
			}
		private:
			inline time_info &clamped() {
				Clamp(hour, u8(0), u8(23));
				Clamp(minute, u8(0), u8(59));
				Clamp(second, u8(0), u8(59));
				return *this;
			}
		} time;
	private:
		void init(tm *info) {
			date.day = (u8)info->tm_mday;
			date.month = (u8)(info->tm_mon + 1);
			date.year = (u32)(1900 + info->tm_year);

			time.hour = (u8)(info->tm_hour);
			time.minute = (u8)(info->tm_min);
			time.second = (u8)(info->tm_sec);
		}

		void init_now() {
			auto now = std::time(nullptr);
			auto info = std::localtime(&now);
			init(info);
		}
	};

	std::ostream &operator<<(std::ostream &out, const util::date_time &data) {
		std::ios guard{ {} };
		guard.copyfmt(out);

		auto out_forcew = [&guard, &out](auto data, size_t width) {
			out << std::setfill('0');
			out << std::setw(width);
			// it interprets bytes as utf-8 chars for some reason
			out << (u32)data;

			// reset to default
			out.copyfmt(guard);
		};

		if (DATE_PRINT_FORMAT == util::DDMMYYYY) {
			out_forcew(data.date.day, 2);
			out << "/";
			out << data.date.month;
			out << "/";
		} else {
			out_forcew(data.date.month, 2);
			out << "/";
			out << data.date.day;
			out << "/";
		}

		out_forcew(data.date.year, 4);

		out << " at ";
		out_forcew(data.time.hour, 2);
		out << ":";
		out << data.time.minute;
		out << ":";
		out << data.time.second;
		return out;
	}

	std::istream &operator>>(std::istream &in, util::date_time &target) {
		std::string day, month, year;
		std::cout << "Enter day:\n";
		in >> day;
		std::cout << "Enter month\n";
		in >> month;
		std::cout << "Enter year:\n";
		in >> year;

		target = util::date_time(day.append("/").append(month).append("/").append(year));
		return in;
	}
} // namespace util

#endif DATETIME_H // DATETIME_H