#pragma once
#include <charconv>
#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
concept InputTransform = requires(T t, std::ifstream &s) {
  t(s);
};

auto noop = [](auto x) { return x; };
auto full_file(auto transform) {
  return [=](std::ifstream &file) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    return transform(buffer.str());
  };
}
auto full_file() { return full_file(noop); }
template <class> inline constexpr bool fals = false;
template <typename TR> auto split_by(char delim = '\n', TR transform = noop) {
  if constexpr (std::invocable<TR, std::ifstream &>)
    return [=](std::ifstream &file) {
      using R = std::invoke_result_t<TR, std::ifstream &>;
      std::vector<R> result;
      do {
        result.push_back(transform(file));
        if (file.peek() != delim)
          break;
        file.ignore(1);
      } while (!file.eof());
      return result;
    };
  else if constexpr (std::invocable<TR, std::string &>)
    return [=](auto &x) {
      using R = std::invoke_result_t<TR, std::string &>;
      using T = std::remove_cvref_t<decltype(x)>;
      if constexpr (std::is_same_v<T, std::ifstream>) {
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
                      "split_by requires a string or ifstream, got a T");
    };
  else
    static_assert(fals<TR>, "split_by requires a transform that takes a string "
                            "or ifstream, got a TR");
}
auto split_by(char delim = '\n') { return split_by(delim, noop); }
auto lines(auto transform) { return split_by('\n', transform); }
auto lines() { return lines(noop); }
auto pair_transform(char delim, auto transform) {
  return [=](auto &x) {
    auto splitter = split_by(delim, transform);
    auto parts = splitter(x);
    if (parts.size() != 2)
      throw std::runtime_error("pair transform expected 2 parts, got " +
                               std::to_string(parts.size()));
    return std::make_pair(parts[0], parts[1]);
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
template <typename T = int> auto s_to_num(std::ifstream &input) {
  T number;
  input >> number;
  return number;
}