#include "shared.h"

using namespace std;
int main() {
  day(
      lines(to_num<uint>()),
      [&](std::vector<uint> s) {
        uint count = 0;
        uint prev = -1;
        for (auto n : s) {
          if (n > prev)
            count++;
          prev = n;
        }
        return count;
      },
      [&](std::vector<uint> s) {
        uint count = 0;
        for (uint i = 0; i < s.size() - 3; i++)
          if (s[i + 3] > s[i])
            count++;
        return count;
      });
}