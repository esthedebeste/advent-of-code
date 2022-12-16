#include "shared.h"
#include <array>
#include <numeric>

using namespace std;
#define sum(arr) accumulate(arr.begin(), arr.end(), uint64_t(0))

int main() {
  array<uint64_t, 9> stages;
  stages.fill(0);
  day(
      [&](istream &file) {
        do
          stages[file.get() - '0']++;
        while (file.get() != EOF);
        return stages;
      },
      [&](auto _) {
        for (uint day = 0; day < 80; day++) {
          uint64_t new_fish = stages[0];
          for (uint64_t j = 0; j < 8; j++)
            stages[j] = stages[j + 1];
          stages[8] = new_fish;
          stages[6] += new_fish;
        }
        return sum(stages);
      },
      [&](auto _) {
        for (uint day = 0; day < 256 - 80 /* already did 80 days */; day++) {
          uint64_t new_fish = stages[0];
          for (uint j = 0; j < 8; j++)
            stages[j] = stages[j + 1];
          stages[8] = new_fish;
          stages[6] += new_fish;
        }
        return sum(stages);
      });
}