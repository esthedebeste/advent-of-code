#include "shared.h"
#include <algorithm>
#include <ranges>
#include <variant>

using namespace std;

// using can't self-reference, so we have to use a struct extends.
struct value {
  variant<int, vector<value>> data;
  value(int data) : data{data} {}
  value(vector<value> data) : data{data} {}
  value() : data(0) {}

  // friends :)
  friend bool operator<(vector<value> a, int b);
  friend bool operator<(int a, vector<value> b);
  friend bool operator<(vector<value> a, vector<value> b);
  friend bool operator<(value a, value b);
};
bool operator<(vector<value> a, int b) {
  if (a.empty())
    return true;
  return a[0] < value(b);
}
bool operator<(int a, vector<value> b) {
  if (b.empty())
    return false;
  return value{a} < b[0];
}
bool operator<(vector<value> a, vector<value> b) {
  auto m = min(a.size(), b.size());
  for (int i = 0; i < m; i++)
    if (a[i] < b[i])
      return true;
    else if (b[i] < a[i])
      return false;
  return a.size() < b.size();
}
bool operator<(value a, value b) {
  return visit([](auto a, auto b) { return a < b; }, a.data, b.data);
}

using input = vector<pair<value, value>>;
istream &operator>>(istream &input, value &v) {
  if (isdigit(input.peek())) {
    input >> v.data.emplace<int>();
    return input;
  }
  // account for empty vector
  input >> "[";
  auto &results = v.data.emplace<vector<value>>();
  if (input.peek() == ']') { // empty vector
    input >> "]";
    return input;
  }
  do {
    value val{};
    input >> val;
    results.push_back(val);
  } while (input.peek() == ',' && (input >> ","));
  input >> "]";
  return input;
}

int main() {
  day(
      split_by("\n\n", pair_transform('\n', parse<value>)),
      [](const input &input) {
        vector<size_t> correct_indices;
        for (size_t i = 0; i < input.size(); i++) {
          auto [a, b] = input[i];
          if (a < b)
            correct_indices.push_back(i);
        }
        size_t total = 0;
        for (auto i : correct_indices)
          total += i + 1;
        return total;
      },
      [](const input &input) {
        vector<value> all;
        for (auto [a, b] : input) {
          all.push_back(a);
          all.push_back(b);
        }
        sort(all.begin(), all.end());
        auto div1 = value(2);
        auto div2 = value(6);
        size_t div1i = 1, div2i = 1; // 1-based indexing
        bool s1 = false, s2 = false;
        for (auto val : all) {
          if (val < div1)
            div1i++;
          else
            s1 = true;
          if (val < div2)
            div2i++;
          else
            s2 = true;
        }
        // an extra +1 to div2 because div1 offsets the position of div2.
        return div1i * (div2i + 1);
      });
}