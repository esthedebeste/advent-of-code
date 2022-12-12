#pragma once
#include <istream>
#include <limits>
/// usage: input >> "Index: " >> index;
/// skip a string and throw if it doesn't match
template <size_t Len>
std::istream &operator>>(std::istream &in, const char (&s)[Len]) {
  for (size_t i = 0; i < Len - 1; i++) // -1 to skip the null terminator
    if (in.get() != s[i])
      throw std::invalid_argument("Invalid input");
  return in;
}
struct skip_until {
  char c;
  constexpr skip_until(char c) : c{c} {}
};

std::istream &operator>>(std::istream &in, skip_until s) {
  in.ignore(std::numeric_limits<std::streamsize>::max(), s.c);
  return in;
}

#include <utility>
template <typename T1, typename T2> struct std::hash<std::pair<T1, T2>> {
  size_t operator()(const pair<T1, T2> &p) const {
    return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
  }
};