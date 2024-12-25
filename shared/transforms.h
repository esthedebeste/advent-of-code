#pragma once
#include <sstream>
#include <string>
#include <type_traits>
#include <concepts>
#include <vector>
#include "utils.h"

template <class T>
concept InputTransform = requires(T t, std::istream &s) { t(s); } || requires(T t, std::string_view s) { t(s); };

inline auto just_a_string = [](const std::string &arg) -> std::string { return arg; };

inline auto full_file() {
	return [](const std::string_view str) { return std::string{str}; };
}
template <class> inline constexpr bool fals = false;

template <InputTransform TR, size_t Len> auto split_by(const char (&delim)[Len], TR transform) {
	return [=](std::istream &file) {
		using R = std::invoke_result_t<TR, std::istream &>;
		std::vector<R> result;
		do result.push_back(transform(file));
		while (file.peek() != EOF && check(file, delim));
		return result;
	};
}

template <class TR> auto split_by(char delim = '\n', TR transform = just_a_string) {
	if constexpr (std::invocable<TR, std::istream &>)
		return [=](std::istream &file) {
			using R = std::invoke_result_t<TR, std::istream &>;
			std::vector<R> result;
			do result.push_back(transform(file));
			while (check(file, delim));
			return result;
		};
	else if constexpr (std::invocable<TR, std::string &>)
		return [=]<class RF>(RF &x) {
			using R = std::invoke_result_t<TR, std::string &>;
			using T = std::remove_cvref_t<RF>;
			if constexpr (std::is_same_v<T, std::istream>) {
				std::vector<R> lines;
				std::string line;
				while (std::getline(x, line, delim)) lines.push_back(transform(line));
				return lines;
			} else if constexpr (std::is_same_v<T, std::string>) {
				std::vector<R> lines;
				std::string line;
				std::istringstream stream(x);
				while (std::getline(stream, line, delim)) lines.push_back(transform(line));
				return lines;
			} else
				static_assert(fals<T>, "split_by requires a string or istream, got a T");
		};
	else
		static_assert(fals<TR>,
									"split_by requires a transform that takes a string "
									"or istream, got a TR");
}

inline auto split_by(const char delim = '\n') { return split_by(delim, just_a_string); }

auto lines(auto transform) { return split_by('\n', transform); }
inline auto lines() { return lines(just_a_string); }

auto noskipws(auto transform) {
	return [=](std::istream &file) {
		file >> std::noskipws;
		auto result = transform(file);
		return result;
	};
}

auto pair_transform(const char delim, InputTransform auto transform) {
	return [=](std::istream &x) {
		auto a = transform(x);
		if (x.peek() != delim)
			throw std::runtime_error("pair transform expected delim, got " + std::to_string(x.peek()));
		x.ignore(1);
		auto b = transform(x);
		return std::make_pair(a, b);
	};
}

template <class T = int> auto parse_num(std::string_view str, int base = 10) {
	T number;
	std::from_chars(str.data(), str.data() + str.size(), number, base);
	return number;
}

template <class T = int> auto parse(std::istream &input) {
	T parsed{};
	input >> parsed;
	return parsed;
}

template <class N = int> auto to_num(int base = 10) {
	return [=]<class T>(T &str) {
		using R = std::remove_cvref_t<T>;
		if constexpr (std::is_same_v<R, std::string_view>) {
			return parse_num<N>(str, base);
		}
		if constexpr (std::is_same_v<R, std::istream>) {
			return parse<N>(str);
		}
	};
}
