#include "shared.h"

using u2 = uint_fast8_t;
using u4 = uint_fast8_t;
struct Game {
  u2 opponent : 2;
  u2 move : 2;
  u4 unused : 4;
} __attribute__((packed));

using namespace std;
int main() {
  using input = vector<Game> &;
  day(
      [](istream &input) {
        vector<Game> games;
        while (!input.eof()) {
          u2 opponent = input.get() - 'A';
          input.ignore(1); // space
          u2 own = input.get() - 'X';
          input.ignore(1); // newline
          games.push_back({opponent, own});
        }
        return games;
      },
      [](input games) {
        uint64_t score = 0;
        for (auto game : games) {
          score += game.move + 1;
          if (game.move == game.opponent)
            score += 3;
          else if ((game.move + 3 - 1) % 3 == game.opponent)
            score += 6;
        }
        return score;
      },
      [](input games) {
        uint64_t score = 0;
        for (auto game : games) {
          u2 move;
          switch (game.move) {
          case 0: // lose
            move = (game.opponent - 1 + 3) % 3;
            break;
          case 1: // draw
            move = game.opponent;
            break;
          case 2: // win
            move = (game.opponent + 1) % 3;
            break;
          }
          score += move + 1 + game.move * 3;
        }
        return score;
      });
}