#include "shared.h"

enum Dir { X, Y };
struct Instruction {
  Dir dir;
  int dist;
};

using namespace std;
int main() {
  day(
      lines([&](string_view str) -> Instruction {
        switch (str[0]) {
        case 'f':
          return {X, parse_num(str.substr("forward "s.length()))};
        case 'd':
          return {Y, parse_num(str.substr("down "s.length()))};
        case 'u':
          return {Y, -parse_num(str.substr("up "s.length()))};
        default:
          std::cerr << "Unknown instruction: " << str << std::endl;
          std::exit(1);
        }
      }),
      [&](vector<Instruction> ins) {
        int x = 0, depth = 0;
        for (auto [dir, dist] : ins) {
          if (dir == X)
            x += dist;
          else
            depth += dist;
        }
        return x * depth;
      },
      [&](vector<Instruction> ins) {
        int x = 0, depth = 0, aim = 0;
        for (auto [dir, dist] : ins) {
          if (dir == Y)
            aim += dist;
          else {
            x += dist;
            depth += aim * dist;
          }
        }
        return x * depth;
      });
}
