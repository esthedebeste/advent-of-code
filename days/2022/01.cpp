#include "shared.h"
#include <algorithm>

using namespace std;
int main() {
  using input = vector<uint> &;
  day(
      [](ifstream &input) {
        vector<uint> elves;
        while (!input.eof()) {
          uint elf = 0;
          while (input.peek() != '\n' && !input.eof()) {
            uint cals = 0;
            input >> cals;
            elf += cals;
            input.get(); // skip newline
          }
          input.get(); // skip newline
          elves.push_back(elf);
        }
        sort(elves.begin(), elves.end(), greater<uint>());
        return elves;
      },
      [](input elves) { return elves[0]; },
      [](input elves) { return elves[0] + elves[1] + elves[2]; });
}