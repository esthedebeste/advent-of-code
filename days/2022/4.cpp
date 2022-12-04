#include "shared.h"

using namespace std;

int main() {
  using assignment = pair<uint, uint>; // (from, to)
  using assignmentP = pair<assignment, assignment>;
  using input = vector<assignmentP> &;
  day(
      lines(pair_transform(',', pair_transform('-', s_to_num<uint>))),
      [](input input) {
        uint64_t count = 0;
        for (auto [a, b] : input)
          if ((a.first <= b.first && a.second >= b.second)     // a contains b
              || (a.first >= b.first && a.second <= b.second)) // b contains a
            count++;
        return count;
      },
      [](input input) {
        uint64_t overlaps = 0;
        for (auto [a, b] : input)
          if (a.first <= b.second && a.second >= b.first)
            overlaps++;
        return overlaps;
      });
}
