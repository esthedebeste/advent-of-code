#pragma once
#include <algorithm>
#include <chrono>
#include <cmath>
#include <concepts>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <version>
#include "transforms.h"
#include "utils.h"
#if !defined(AOC_YEAR) || !defined(AOC_DAY)
#error AOC_YEAR and AOC_DAY must be defined
#endif

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define AOC_YEAR_STR STRINGIFY(AOC_YEAR)
#define AOC_DAY_STR STRINGIFY(AOC_DAY)

using uint = unsigned int;
using dmilli = std::chrono::duration<double, std::milli>;

void timeit(const std::string_view name, auto func) {
	const auto start = std::chrono::high_resolution_clock::now();
	func();
	const auto end = std::chrono::high_resolution_clock::now();
	std::cout << "TIMEIT: " << name << ": "
						<< std::format("{:L}", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start)) << "\n";
}

namespace internal {
	really_always_inline auto parse_input(InputTransform auto transform) {
#ifndef AOC_INPUT_PATH
		const std::string path = "./input/" AOC_YEAR_STR "/" AOC_DAY_STR ".txt";
		std::cout << "Opening " << path << "..." << std::flush;
		std::ifstream file(path, std::ios_base::binary);
		if (!file.is_open()) {
			std::cerr << "Could not open file: " << path;
			exit(1);
		}
		std::istream &istream = file;
#endif
		if constexpr (std::invocable<decltype(transform), std::istream &>) {
#ifdef AOC_INPUT_PATH
			std::istringstream inputstring(
#include AOC_INPUT_PATH


				, std::ios_base::binary);
			std::istream &istream = inputstring;
#endif
			return std::move(transform(istream));
		} else {
#ifndef AOC_INPUT_PATH
			std::stringstream buffer;
			buffer << file.rdbuf();
			std::string str = buffer.str();
			return std::move(transform(str));
#else
			return std::move(transform(std::string_view(
#include AOC_INPUT_PATH


				)));
#endif
		}
	}

	really_always_inline auto timeit(auto task, const std::string_view start_str, const std::string_view end_str,
																	 std::chrono::high_resolution_clock::duration &total) {
		std::cout << start_str << std::endl;
		const auto start = std::chrono::high_resolution_clock::now();
		auto res = task();
		const auto end = std::chrono::high_resolution_clock::now();
		std::cout << end_str << " (" << std::chrono::duration_cast<dmilli>(end - start).count() << "ms)" << std::endl;
		total += end - start;
		return res;
	}
}

void day(InputTransform auto transform, auto... func) {
	std::ios::sync_with_stdio(false);
	std::cout << "Running day " AOC_DAY_STR "..." << std::endl;
	std::chrono::high_resolution_clock::duration total_time{};
	auto input =
		internal::timeit([&] { return internal::parse_input(transform); }, "Processing input...", "Done processing input!", total_time);
	int currPart = 0;
	(
		[&] {
			currPart++;
			std::string start_str = "Running part " + std::to_string(currPart) + "...";
			std::string end_str = "Done with part " + std::to_string(currPart) + "!";
			auto res = internal::timeit([&] { return func(input); }, start_str, end_str, total_time);
			std::cout << "Part " << currPart << " result: " << res << std::endl;
		}(),
		...);
	std::cout << "Done running day " AOC_DAY_STR " in " << std::chrono::duration_cast<dmilli>(total_time).count() << "ms!\n";
}
