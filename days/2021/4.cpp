#include "shared.h"
#include <forward_list>

using namespace std;

struct BingoBoard {
  uint content[5][5];
  uint32_t enabled_mask = 0; // 25 bits
  auto &operator[](auto y) { return content[y]; }
  bool is_enabled(uint y, uint x) { return enabled_mask & (1 << (y * 5 + x)); }
  bool set_and_check_win(uint n) {
    for (uint y = 0; y < 5; y++)
      for (uint x = 0; x < 5; x++)
        if (content[y][x] == n) {
          enabled_mask |= 1 << (y * 5 + x);
          for (uint x = 0; x < 5; x++) // row
            if (!is_enabled(y, x))
              goto column;
          return true;
        column:
          for (uint y = 0; y < 5; y++)
            if (!is_enabled(y, x))
              return false;
          return true;
        }
    return false;
  }
  uint unmarked_sum() {
    uint sum = 0;
    for (uint y = 0; y < 5; y++)
      for (uint x = 0; x < 5; x++)
        if (!is_enabled(y, x))
          sum += content[y][x];
    return sum;
  }
};

struct BingoGame {
  vector<uint> numbers;
  vector<BingoBoard> boards;
};

BingoGame parse_bingo_game(ifstream &file) {
  BingoGame game;
  do {
    uint n;
    file >> n;
    game.numbers.push_back(n);
    file.ignore(1); // skip ,
  } while (file.peek() != '\n');
  file.ignore(1);
  do {
    BingoBoard board;
    for (uint y = 0; y < 5; y++) {
      for (uint x = 0; x < 5; x++) {
        while (file.peek() == ' ')
          file.ignore(1);
        file >> board[y][x];
      }
      file.ignore(1); // newline
    }
    game.boards.push_back(board);
    file.ignore(1); // newlines
  } while (file);
  return game;
}

int main() {
  uint part2start;
  day(
      parse_bingo_game,
      [&](BingoGame &game) -> uint {
        for (uint i = 0; i < game.numbers.size(); i++) {
          uint n = game.numbers[i];
          for (auto &board : game.boards)
            if (board.set_and_check_win(n)) {
              part2start = i;
              return board.unmarked_sum() * n;
            }
        }
        unreachable();
      },
      [&](BingoGame &game) -> uint {
        forward_list<BingoBoard *> boards;
        for (auto &board : game.boards)
          boards.push_front(&board);

        for (uint i = 0; i < game.numbers.size(); i++) {
          uint n = game.numbers[i];
          auto pboard = boards.before_begin();
          for (auto it = boards.begin(); it != boards.end();) {
            auto board = *it;

            if (board->set_and_check_win(n)) {
              it = boards.erase_after(pboard);
              if (boards.empty())
                return board->unmarked_sum() * n;
            } else {
              pboard = it;
              it++;
            }
          }
        }
        unreachable();
      });
}
