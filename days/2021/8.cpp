// lmao this does Not work
#include "shared.h"
#include <array>
#include <bitset>
#include <map>
using namespace std;

using u8 = uint_fast8_t;

u8 index(char c) {
  if (c < 'a' || c > 'h')
    throw invalid_argument("Invalid index");
  return 1 << (c - 'a');
}
bool index(u8 sevSeg, u8 index) { return (sevSeg >> index) & 1; }
vector<u8> indices(u8 sevSeg) {
  vector<u8> result;
  result.reserve(7);
  for (u8 i = 0; i < 7; ++i)
    if (index(sevSeg, i))
      result.push_back(i);
  return result;
}
u8 length(u8 sevSeg) {
  u8 len = 0;
  while (sevSeg) {
    len += sevSeg & 1;
    sevSeg >>= 1;
  }
  return len;
}
u8 first(auto &input, auto pred) {
  for (auto sevSeg : input)
    if (pred(sevSeg))
      return sevSeg;
  throw invalid_argument("No matching element");
}

u8 firstOfLength(auto &input, u8 len) {
  return first(input, [=](u8 sevSeg) { return length(sevSeg) == len; });
}
template <size_t T> array<u8, T> spaceSplit(string &input) {
  array<u8, T> lines;
  istringstream stream(input);
  for (u8 i = 0; i < T; i++) {
    string str;
    getline(stream, str, ' ');
    u8 sevSeg = 0;
    for (auto c : str)
      sevSeg |= index(c);
    lines[i] = sevSeg;
  }
  return lines;
}
map<u8, u8> genMappings(array<u8, 10> &input) {
  u8 one = firstOfLength(input, 2), two, three, four = firstOfLength(input, 4),
     five, six, seven = firstOfLength(input, 3),
     eight = firstOfLength(input, 7), nine, zero;

  u8 middleTopLeftLines = (eight ^ seven) & four;
  auto middleTopLeft = [=](u8 sevSeg) {
    return (sevSeg & middleTopLeftLines) == middleTopLeftLines;
  };
  {
    u8 nineSixZero = firstOfLength(input, 6);
    u8 sixZeroNine = first(input, [=](u8 sevSeg) {
      return length(sevSeg) == 6 && sevSeg != nineSixZero;
    });
    u8 zeroNineSix = first(input, [=](u8 sevSeg) {
      return length(sevSeg) == 6 && sevSeg != nineSixZero &&
             sevSeg != sixZeroNine;
    });
    // 6 and 9 both have the middle and the
    // left-side top line on.
    u8 nineSix, sixNine;
    if (!middleTopLeft(nineSixZero))
      nineSix = zeroNineSix, sixNine = sixZeroNine, zero = nineSixZero;
    else if (!middleTopLeft(sixZeroNine))
      nineSix = zeroNineSix, sixNine = nineSixZero, zero = sixZeroNine;
    else
      nineSix = sixZeroNine, sixNine = nineSixZero, zero = zeroNineSix;
    if ((nineSix & one) == one)
      nine = nineSix, six = sixNine;
    else
      nine = sixNine, six = nineSix;
  }
  // 1, 4, 6, 7, 8, 9, 0 now defined, 2, 3 and 5 to go.
  {
    u8 twoThreeFive = firstOfLength(input, 5);
    u8 threeFiveTwo = first(input, [=](u8 sevSeg) {
      return length(sevSeg) == 5 && sevSeg != twoThreeFive;
    });
    u8 fiveTwoThree = first(input, [=](u8 sevSeg) {
      return length(sevSeg) == 5 && sevSeg != twoThreeFive &&
             sevSeg != threeFiveTwo;
    });
    u8 twoThree, threeTwo;
    // five is the only one with the middle line on.
    if (middleTopLeft(twoThreeFive))
      twoThree = fiveTwoThree, threeTwo = threeFiveTwo, five = twoThreeFive;
    else if (middleTopLeft(threeFiveTwo))
      twoThree = fiveTwoThree, threeTwo = twoThreeFive, five = threeFiveTwo;
    else
      twoThree = threeFiveTwo, threeTwo = twoThreeFive, five = fiveTwoThree;

    if ((twoThree & one) == one)
      three = twoThree, two = threeTwo;
    else
      two = threeTwo, three = twoThree;
  }
  return {{zero, 0}, {one, 1}, {two, 2},   {three, 3}, {four, 4},
          {five, 5}, {six, 6}, {seven, 7}, {eight, 8}, {nine, 9}};
}

int main() {
  using input_t = vector<pair<array<u8, 10>, array<u8, 4>>>;
  day(
      lines([](string line) {
        auto delimPos = 59; // line.find_first_of('|')
        auto leftSide = string{begin(line), begin(line) + delimPos - 1};
        auto rightSide = string{begin(line) + delimPos + 2, end(line)};
        return make_pair(spaceSplit<10>(leftSide), spaceSplit<4>(rightSide));
      }),
      [&](input_t &input) {
        int count = 0;
        for (auto &line : input) {
          auto &[left, right] = line;
          for (auto sevSeg : right) {
            auto len = length(sevSeg);
            if (len == 2 || len == 3 || len == 4 || len == 7)
              count++;
          }
        }
        return count;
      },
      [&](input_t &input) {
        uint64_t sum = 0;
        for (auto &line : input) {
          auto &[left, right] = line;
          auto mappings = genMappings(left);
          uint64_t num = 0;
          for (auto sevSeg : right)
            num = num * 10 + mappings[sevSeg];
          sum += num;
        }
        return sum;
      });
}