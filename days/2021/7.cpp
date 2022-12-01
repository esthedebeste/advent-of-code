#include "shared.h"
#include <algorithm>
#include <limits>

using namespace std;

long crab_mechanics(long n) {
  return (n * (n + 1)) / 2; // sum(1 + 2 + 3 + ... + n)
}

int main() {
  long min_pos, max_pos;
  day(
      split_by(',', to_num<long>()),
      [&](std::vector<long> crabs) {
        min_pos = *min_element(crabs.begin(), crabs.end());
        max_pos = *max_element(crabs.begin(), crabs.end());
        long min_x = 0, min_fuel = numeric_limits<long>::max();

        for (long x = min_pos; x <= max_pos; x++) {
          long fuel = 0;
          for (auto crab : crabs)
            fuel += abs(crab - x);
          if (fuel < min_fuel) {
            min_fuel = fuel;
            min_x = x;
          }
        }
        return min_fuel;
      },
      [&](std::vector<long> crabs) {
        long min_x = 0, min_fuel = numeric_limits<long>::max();

        for (long x = min_pos; x <= max_pos; x++) {
          long fuel = 0;
          for (auto crab : crabs)
            fuel += crab_mechanics(abs(crab - x));
          if (fuel < min_fuel) {
            min_fuel = fuel;
            min_x = x;
          }
        }
        return min_fuel;
      });
}