#include "shared.h"

int main() {
  // i did this one already :)
  day(
      lines(to_num()), [](auto ins) { return ins[0]; },
      [](auto ins) { return ins[1]; });
}