#include "shared.h"
#include <limits>
#include <optional>

using namespace std;

struct Move {
  uint from, to, amount;
};

using Stack = vector<char>;

struct Input {
  vector<Stack> stacks;
  vector<Move> moves;
  string tops() {
    string res;
    for (auto &stack : stacks)
      res += stack.back();
    return res;
  }
};

vector<Stack> parse_stacks(ifstream &input) {
  vector<vector<char>> hors;
  while (true) {
    vector<char> hor;
    do
      switch (input.get()) {
      case '[':
        hor.push_back(input.get());
        input.ignore(1); // ]
        break;
      case ' ':
        if (isdigit(input.peek())) {
          // skip rest of line and newline in between stacks and moves
          input.ignore(numeric_limits<streamsize>::max(), '\n');
          input.ignore(1);
          goto ret;
        }
        hor.push_back(0);
        input.ignore(2); // two other spaces
        break;
      default:
        throw runtime_error("unexpected char");
      }
    while (input.get() == ' ');
    hors.push_back(hor);
  }
ret:
  vector<Stack> stacks;
  auto width = hors[0].size();
  for (size_t i = 0; i < width; i++) {
    vector<char> top;
    for (auto it = hors.rbegin(); it != hors.rend(); it++) // reverse
      if (auto c = (*it)[i])
        top.push_back(c);
    stacks.push_back({top});
  }
  return stacks;
}

int main() {
  day(
      [](ifstream &input) {
        Input result;
        result.stacks = parse_stacks(input);
        do {
          Move move;
          input.ignore(5); // "move "
          input >> move.amount;
          input.ignore(6); // " from "
          input >> move.from;
          input.ignore(4); // " to "
          input >> move.to;
          // turn into 0-indexed
          move.from--;
          move.to--;
          result.moves.push_back(move);
        } while (input.get() != EOF);
        return result;
      },
      // dont use ref! we need to modify input
      [](Input input) {
        for (auto &move : input.moves) {
          auto &from = input.stacks[move.from];
          auto &to = input.stacks[move.to];
          for (uint i = 0; i < move.amount; i++) {
            to.push_back(from.back());
            from.pop_back();
          }
        }
        return input.tops();
      },
      [](Input input) {
        for (auto &move : input.moves) {
          auto &from = input.stacks[move.from];
          auto &to = input.stacks[move.to];
          to.insert(to.end(), from.end() - move.amount, from.end());
          from.erase(from.end() - move.amount, from.end());
        }
        return input.tops();
      });
}
