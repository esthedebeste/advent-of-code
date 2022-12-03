#include "shared.h"

using namespace std;
using Rucksack = pair<uint64_t, uint64_t>;

auto substr(string_view x, size_t begin, size_t length) {
  return x.substr(begin, length);
}
auto substr(string_view x, size_t begin) { return x.substr(begin); }

uint64_t item_to_score(uint64_t item) {
  for (uint64_t i = 0; i < 64; i++)
    if (item == (1ull << i))
      return i;
  throw runtime_error("item_to_score: item not found (multiple or 0 items)");
}

auto shared(Rucksack sack, auto... sacks) {
  uint64_t full = sack.first | sack.second;
  for (auto other : {sacks...})
    full &= other.first | other.second;
  return full;
}

auto shared(uint64_t pocket, auto... pockets) {
  uint64_t full = pocket;
  for (auto other : {pockets...})
    full &= other;
  return full;
}

int main() {
  using input = vector<Rucksack> &;
  day(
      lines([](string str) {
        auto a = substr(str, 0, str.length() / 2);
        auto b = substr(str, str.length() / 2);
        uint64_t a_bin = 0;
        for (char c : a)
          a_bin |= 1ull << (c >= 'a' ? (c - 'a' + 1) : (c - 'A' + 27));
        uint64_t b_bin = 0;
        for (char c : b)
          b_bin |= 1ull << (c >= 'a' ? (c - 'a' + 1) : (c - 'A' + 27));
        return Rucksack{a_bin, b_bin};
      }),
      [](input sacks) {
        uint64_t sum = 0;
        for (auto [first, second] : sacks)
          sum += item_to_score(shared(first, second));
        return sum;
      },
      [](input sacks) {
        uint64_t sum = 0;
        for (size_t i = 0; i < sacks.size(); i += 3)
          sum += item_to_score(shared(sacks[i], sacks[i + 1], sacks[i + 2]));
        return sum;
      });
}