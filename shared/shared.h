#pragma once
#include "transforms.h"
#include "utils.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <format>
#include <algorithm>
#include <ranges>
#include <numeric>
#include <cmath>
#include <version>
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
	std::cout << "TIMEIT: " << name << ": " << std::format(
			"{:L}",
			std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
				end - start)) <<
		"\n";
}

void day(InputTransform auto transform, auto... func) {
	std::ios::sync_with_stdio(false);
	std::cout << "Running day " AOC_DAY_STR "..." << std::endl;
	std::cout << "Processing input..." << std::endl;
	std::chrono::high_resolution_clock::duration total_time{};
	const auto start = std::chrono::high_resolution_clock::now();
	auto input = [&] {
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
	}();
	const auto end = std::chrono::high_resolution_clock::now();
	total_time += end - start;
	std::cout << "Done processing input! ("
		<< std::chrono::duration_cast<dmilli>(end - start).count() << "ms)"
		<< std::endl;
	int currPart = 0;
	(
		[&] {
			currPart++;
			std::cout << "Running part " << currPart << "..." << std::endl;
			const auto start = std::chrono::high_resolution_clock::now();
			auto res = func(input);
			const auto end = std::chrono::high_resolution_clock::now();
			total_time += end - start;
			std::cout << "Done with part " << currPart << "! Result: `" << res <<
				"` (" << std::chrono::duration_cast<dmilli>(end - start).
				count() << "ms)" << std::endl;
		}(),
		...);
	std::cout << "Done running day " AOC_DAY_STR " in " <<
		std::chrono::duration_cast<dmilli>(total_time).
				count() << "ms!\n";
}
