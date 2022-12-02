#pragma once
#include "transforms.h"
#include <fstream>
#include <iostream>
#include <version>
#if !defined(AOC_YEAR) || !defined(AOC_DAY)
#error AOC_YEAR and AOC_DAY must be defined
#endif

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define AOC_YEAR_STR STRINGIFY(AOC_YEAR)
#define AOC_DAY_STR STRINGIFY(AOC_DAY)

using uint = unsigned int;

template <InputTransform Transform, typename... Funcs>
void day(Transform transform, Funcs... func) {
  std::cout << "Running day " AOC_DAY_STR "..." << std::endl;
  int currPart = 0;
  std::string path = "../input/" AOC_YEAR_STR "/" AOC_DAY_STR ".txt";
  std::cout << "Opening " << path << "..." << std::flush;
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Could not open file: " << path;
    return;
  }
  std::cout << "\rTransforming input...        " << std::flush;
  auto input = transform(file);
  std::cout << "\rRead and transformed input...    " << std::endl;
  (
      [&] {
        currPart++;
        std::cout << "Running part " << currPart << "..." << std::endl;
        auto res = func(input);
        std::cout << "Done running part " << currPart << ", "
                  << "result: '" << res << "'\n";
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