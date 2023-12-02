#pragma once
#include "utils.h"
#include <charconv>
#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
concept InputTransform = requires(T t, std::istream &s) { t(s); };

auto noop = [](std::string x) { return x; };

auto full_file(auto transform) {
	return [=](std::istream &file) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		return transform(buffer.str());
	};
}

auto full_file() { return full_file(noop); }
template <class> inline constexpr bool fals = false;

template <InputTransform TR, size_t Len>
auto split_by(const char (&delim)[Len], TR transform) {
	return [=](std::istream &file) {
		using R = std::invoke_result_t<TR, std::istream &>;
		std::vector<R> result;
		do
			result.push_back(transform(file));
		while (file.peek() != EOF && file.peek() == delim[0] && file >> delim);
		return result;
	};
}

template <typename TR> auto split_by(char delim = '\n', TR transform = noop) {
	if constexpr (std::invocable<TR, std::istream &>)
		return [=](std::istream &file) {
			using R = std::invoke_result_t<TR, std::istream &>;
			std::vector<R> result;
			do
				result.push_back(transform(file));
			while (file.peek() == delim && file.ignore(1));
			return result;
		};
	else if constexpr (std::invocable<TR, std::string &>)
		return [=](auto &x) {
			using R = std::invoke_result_t<TR, std::string &>;
			using T = std::remove_cvref_t<decltype(x)>;
			if constexpr (std::is_same_v<T, std::istream>) {
				std::vector<R> lines;
				std::string line;
				while (std::getline(x, line, delim))
					lines.push_back(transform(line));
				return lines;
			} else if constexpr (std::is_same_v<T, std::string>) {
				std::vector<R> lines;
				std::string line;
				std::istringstream stream(x);
				while (std::getline(stream, line, delim))
					lines.push_back(transform(line));
				return lines;
			} else
				static_assert(fals<T>,
				              "split_by requires a string or istream, got a T");
		};
	else
		static_assert(fals<TR>, "split_by requires a transform that takes a string "
		              "or istream, got a TR");
}

auto split_by(char delim = '\n') { return split_by(delim, noop); }

template <InputTransform TR>
auto split_to(char delim = '\n', TR transform = noop) {
	return [=](std::istream &file) {
		using R = std::invoke_result_t<TR, std::istream &>;
		std::vector<R> result;
		do {
			result.push_back(transform(file));
			file >> skip_until(delim);
		} while (file.good());
		return result;
	};
}

auto split_to(auto delim, auto transform) { return split_by(delim, transform); }
auto lines(auto transform) { return split_to('\n', transform); }
auto lines() { return lines(noop); }

auto noskipws(auto transform) {
	return [=](std::istream &file) {
		file >> std::noskipws;
		auto result = transform(file);
		return result;
	};
}

auto pair_transform(char delim, InputTransform auto transform) {
	return [=](std::istream &x) {
		auto a = transform(x);
		if (x.peek() != delim)
			throw std::runtime_error("pair transform expected delim, got " +
			                         std::to_string(x.peek()));
		x.ignore(1);
		auto b = transform(x);
		return std::make_pair(a, b);
	};
}

template <typename T = int>
auto parse_num(std::string_view str, int base = 10) {
	T number;
	std::from_chars(str.data(), str.data() + str.size(), number, base);
	return number;
}

template <typename T = int> auto to_num(int base = 10) {
	return [=](std::string_view str) { return parse_num<T>(str, base); };
}

template <typename T = int> auto parse(std::istream &input) {
	T parsed{};
	input >> parsed;
	return parsed;
}
