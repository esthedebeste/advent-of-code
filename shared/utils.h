#pragma once
#include <istream>
#include <limits>
#include <utility>
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

template <typename T1, typename T2> struct std::hash<std::pair<T1, T2>> {
  size_t operator()(const pair<T1, T2> &p) const {
    return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
  }
};

#ifndef default_pos_t
#define default_pos_t int
#endif
template <std::integral T = default_pos_t> struct pos_t {
  T x, y;
  pos_t<T> up() const { return {x, y - 1}; }
  pos_t<T> down() const { return {x, y + 1}; }
  pos_t<T> left() const { return {x - 1, y}; }
  pos_t<T> right() const { return {x + 1, y}; }
  T manhattan_distance(pos_t<T> other) const {
    return abs(x - other.x) + abs(y - other.y);
  }
  friend bool operator==(pos_t<T> a, pos_t<T> b) {
    return a.x == b.x && a.y == b.y;
  }
  friend bool operator<(pos_t<T> a, pos_t<T> b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
  }
};

namespace std {
template <std::integral T> struct hash<pos_t<T>> {
  size_t operator()(const pos_t<T> &x) const {
    return hash<T>()(x.x) ^ hash<T>()(x.y);
  }
};
}
using pos = pos_t<default_pos_t>;