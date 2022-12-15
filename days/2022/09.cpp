#include "shared.h"
#include <set>

using namespace std;

struct Pos {
  int x, y;
  friend bool operator==(Pos, Pos) = default;
};
bool operator<(Pos a, Pos b) { return a.x < b.x || (a.x == b.x && a.y < b.y); }

enum Dir : char { Left = 'L', Right = 'R', Up = 'U', Down = 'D' };
struct Instruction {
  Dir type;
  int amount;
  void apply(Pos &pos) {
    if (type == Left)
      pos.x--;
    else if (type == Right)
      pos.x++;
    else if (type == Up)
      pos.y++;
    else if (type == Down)
      pos.y--;
  }
};

void tail_part(Pos head, Pos &tail) {
  auto diffx = head.x - tail.x;
  auto diffy = head.y - tail.y;
  if (abs(diffx) <= 1 && abs(diffy) <= 1)
    return;
  if (abs(diffx) > 0)
    tail.x += diffx > 0 ? 1 : -1;
  if (abs(diffy) > 0)
    tail.y += diffy > 0 ? 1 : -1;
}

template <size_t taillen> auto part(vector<Instruction> &instructions) {
  Pos head{0, 0};
  Pos tail[taillen];
  for (uint i = 0; i < taillen; i++)
    tail[i] = head;
  set<Pos> visited;
  for (auto instruction : instructions) {
    for (uint i = 0; i < instruction.amount; i++) {
      instruction.apply(head);
      auto prev = head;
      for (uint i = 0; i < taillen; i++) {
        tail_part(prev, tail[i]);
        prev = tail[i];
      }
      visited.insert(prev);
    }
  }
  return visited.size();
}

int main() {
  using input = vector<Instruction>;
  day(lines([](ifstream &input) {
        char dir = input.get();
        input.ignore(1);
        int value;
        input >> value;
        return Instruction{reinterpret_cast<Dir &>(dir), value};
      }),
      part<1>, part<9>);
}