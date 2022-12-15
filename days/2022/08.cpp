#include "shared.h"
#include <limits>
#include <ranges>

using namespace std;

using field = vector<vector<uint_fast8_t>>;

int main() {
  day(
      lines([](ifstream &input) {
        vector<uint_fast8_t> row;
        while (input.peek() != '\n' && !input.eof())
          row.push_back(input.get() - '0');
        return row;
      }),
      [](field field) {
        int size = field.size();
        int count = size * 4 - 4; // outer edges
        for (int tx = 1; tx < size - 1; tx++)
          for (int ty = 1; ty < size - 1; ty++) {
            int height = field[tx][ty];
            auto do_range = [&](bool base_is_x, int base, int from, int to) {
              for (int x = from; x < to; x++)
                if (field[base_is_x ? base : x][base_is_x ? x : base] >= height)
                  return false;
              count++;
              return true;
            };
            do_range(true, tx, 0, ty) || do_range(true, tx, ty + 1, size) ||
                do_range(false, ty, 0, tx) || do_range(false, ty, tx + 1, size);
          }
        return count;
      },
      [](field field) {
        int size = field.size();
        int optimal = 0;
        for (int tx = 1; tx < size - 1; tx++)
          for (int ty = 1; ty < size - 1; ty++) {
            int height = field[tx][ty];
// macro which is definitely readable (not)
#define calcscore(a, b, c)                                                     \
  int a = 0;                                                                   \
  for (b) {                                                                    \
    a++;                                                                       \
    if (field c >= height)                                                     \
      break;                                                                   \
  }
            calcscore(horPosScore, int y = ty + 1; y < size; y++, [tx][y]);
            calcscore(horNegScore, int y = ty - 1; y >= 0; y--, [tx][y]);
            calcscore(verPosScore, int x = tx + 1; x < size; x++, [x][ty]);
            calcscore(verNegScore, int x = tx - 1; x >= 0; x--, [x][ty]);
            auto score = horPosScore * horNegScore * verPosScore * verNegScore;
            optimal = max(optimal, score);
          }
        return optimal;
      });
}