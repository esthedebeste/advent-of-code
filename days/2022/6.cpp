#include "shared.h"

using namespace std;

int ones(uint32_t n) {
  int count = 0;
  while (n != 0) {
    count += n & 1;
    n >>= 1;
  }
  return count;
}

using input = vector<uint32_t>;

template <size_t length> uint part(input input) {
  uint unique = 0;
  uint start = 0;
  for (uint i = 0; i < input.size(); i++) {
    if (start + length <= i) {
      unique &= ~input[start]; // clear the bit that is too old
      start++;
    }
    if (unique & input[i]) // already seen this character!
      for (uint j = start; j < i; j++)
        if (input[j] == input[i]) { // found the previous occurence
          for (uint k = start; k <= j; k++)
            unique &= ~input[k]; // remove overlap and everything before it
          start = j + 1;         // start at the next element
          break;
        }
    unique |= input[i];
    if (ones(unique) == length)
      return start + length;
  }
  throw runtime_error("no solution");
}

int main() {
  day(
      [](ifstream &input) {
        vector<uint32_t> masks;
        do
          masks.push_back(1 << (input.get() - 'a'));
        while (!input.eof());
        return masks;
      },
      part<4>, part<14>);
}
