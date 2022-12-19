#pragma once
#include "transforms.h"
#include "utils.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <version>
#if !defined(AOC_YEAR) || !defined(AOC_DAY)
#error AOC_YEAR and AOC_DAY must be defined
#endif

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define AOC_YEAR_STR STRINGIFY(AOC_YEAR)
#define AOC_DAY_STR STRINGIFY(AOC_DAY)

using uint = unsigned int;

void day(InputTransform auto transform, auto... func) {
  std::cout << "Running day " AOC_DAY_STR "..." << std::endl;
  int currPart = 0;
#ifndef AOC_INPUT_PATH
  std::string path = "../input/" AOC_YEAR_STR "/" AOC_DAY_STR ".txt";
  std::cout << "Opening " << path << "..." << std::flush;
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Could not open file: " << path;
    return;
  }
  std::istream &istream = file;
#else
  std::istringstream inputstring(
#include AOC_INPUT_PATH
  );
  std::istream &istream = inputstring;
#endif
  std::cout << "Processing input..." << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  auto input = transform(istream);
  auto end = std::chrono::high_resolution_clock::now();
  using dmilli = std::chrono::duration<double, std::milli>;
  std::cout << "Done processing input! ("
            << std::chrono::duration_cast<dmilli>(end - start).count() << "ms)"
            << std::endl;
  (
      [&] {
        currPart++;
        std::cout << "Running part " << currPart << "..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        auto res = func(input);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Done with part " << currPart << "! Result: `" << res
                  << "` ("
                  << std::chrono::duration_cast<dmilli>(end - start).count()
                  << "ms)" << std::endl;
      }(),
      ...);
  std::cout << "Done running day " AOC_DAY_STR "!\n";
}

#if __cpp_lib_unreachable >= 202202L
#define unreachable() std::unreachable() // C++23
#elif defined(__GNUC__)
#define unreachable() __builtin_unreachable(); // GCC and Clang
#elif defined(_MSC_VER)
#define unreachable() __assume(false); // MSVC
#endif
inline constexpr auto max(auto first, auto... args) {
  auto curr = first;
  (..., (curr = curr > args ? curr : args));
  return curr;
}
inline constexpr auto min(auto first, auto... args) {
  auto curr = first;
  (..., (curr = curr < args ? curr : args));
  return curr;
}