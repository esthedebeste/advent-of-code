#include "shared.h"

int main() {
  using input = int &;
  day(
      parse<int>,
      [](input input) {
        // todo, part 1
        return input + 1;
      },
      [](input input) {
        // todo, part 2
        return input + 2;
      });
}