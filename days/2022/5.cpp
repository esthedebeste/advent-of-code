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
};

vector<Stack> parse_stacks(ifstream &input) {
  vector<vector<optional<char>>> hors;
  while (true) {
    vector<optional<char>> hor;
    do {
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
        hor.push_back(nullopt);
        input.ignore(2); // two other spaces
        break;
      default:
        throw runtime_error("unexpected char");
      }
    } while (input.get() == ' ');
    hors.push_back(hor);
  }
ret:
  auto width = hors[0].size();
  vector<Stack> res;
  for (size_t i = 0; i < width; i++) {
    vector<char> top;
    for (auto it = hors.rbegin(); it != hors.rend(); it++)
      if (auto c = (*it)[i]) // not empty
        top.push_back(*c);
    res.push_back({top});
  }
  return res;
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
        string res;
        for (auto &stack : input.stacks)
          res += stack.back();
        return res;
      },
      [](Input input) {
        for (auto &move : input.moves) {
          auto &from = input.stacks[move.from];
          auto &to = input.stacks[move.to];
          to.insert(to.end(), from.end() - move.amount, from.end());
          from.erase(from.end() - move.amount, from.end());
        }
        string res;
        for (auto &stack : input.stacks)
          res += stack.back();
        return res;
      });
}
