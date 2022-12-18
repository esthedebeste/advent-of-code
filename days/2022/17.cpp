#define pos_up 1 // 0 = -, 1 = +
#define default_pos_t int64_t
#include "shared.h"
#include <array>
#include <bitset>
#include <deque>
#include <locale>
#include <numeric>

using namespace std;
using input = vector<bool>;
constexpr size_t width = 7;
struct field : vector<bitset<width>> // [y][x]
{
  bool is_solid(pos p) const {
    return (p.x < 0 || p.x >= width || p.y < 0) // out of bounds is solid
           || (p.y < size()                     // too high is not solid
               && at(p.y)[p.x]);
  }
  void print() const {
    for (ptrdiff_t i = size(); i-- > 0;) {
      const auto &row = at(i);
      cout << '|';
      for (size_t i = 0; i < width; i++)
        cout << (row[i] ? '#' : '.');
      cout << '|' << endl;
    }
    cout << '+' << string(width, '-') << '+' << endl;
  }
};
struct shapeinfo {
  vector<pos> points;
  size_t height = 0;
  shapeinfo(vector<pos> points) : points{points} {
    for (pos p : points)
      height = max(height, p.y);
  }
};
struct shape : public pos {
  const shapeinfo &info;
  field &grid;
  shape(field &grid, const shapeinfo &info, pos pos) : grid{grid}, info{info} {
    x = pos.x;
    y = pos.y;
  }
  bool is_colliding() {
    for (pos rp : info.points)
      if (grid.is_solid(rp + *this))
        return true;
    return false;
  }
  void solidify() {
    for (pos rp : info.points) {
      pos pos = rp + *this;
      if (pos.y >= grid.size())
        grid.resize(pos.y + 1);
      grid[pos.y][pos.x] = true;
    }
  }
};
shapeinfo shapes[]{{{{0, 0}, {1, 0}, {2, 0}, {3, 0}}},         // horizontal
                   {{{1, 0}, {1, 1}, {1, 2}, {0, 1}, {2, 1}}}, // plus
                   {{{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}}}, // corner
                   {{{0, 0}, {0, 1}, {0, 2}, {0, 3}}},         // vertical
                   {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}}};        // square
constexpr bool LEFT = false;
constexpr bool RIGHT = true;

int main() {
  day(
      [](istream &input) {
        vector<bool> movements;
        while (input.peek() != EOF)
          movements.push_back(input.get() == '>' ? RIGHT : LEFT);
        return movements;
      },
      [](input movements) {
        field grid;
        size_t highest = 0;
        size_t curr_move = 0;
        for (uint64_t i = 0; i < 2022; i++) {
          auto shape =
              ::shape{grid, shapes[i % 5], pos{2, int64_t(highest + 3)}};
          while (!shape.is_colliding()) {
            auto move = movements[curr_move++];
            curr_move %= movements.size();
            shape.x += move == RIGHT ? 1 : -1;
            if (shape.is_colliding())
              shape.x += move == RIGHT ? -1 : 1; // undo
            shape.y--;
          }
          shape.y++; // undo last fall (we're on the ground)
          shape.solidify();
          highest = max(highest, shape.y + shape.info.height + 1);
        }
        return highest;
      },
      [](input movements) -> size_t {
        cout.imbue(locale(""));
        field grid;
        size_t highest = 0;
        size_t curr_move = 0;
        size_t prev = 0;
        vector<size_t> pattern_detector;
        constexpr uint64_t rocks_to_simulate = 1'000'000'000'000;
        for (uint64_t i = 0; i < rocks_to_simulate; i++) {
          auto shape =
              ::shape{grid, shapes[i % 5], pos{2, int64_t(highest + 3)}};
          while (!shape.is_colliding()) {
            auto move = movements[curr_move++];
            curr_move %= movements.size();
            shape.x += move == RIGHT ? 1 : -1;
            if (shape.is_colliding())
              shape.x += move == RIGHT ? -1 : 1; // undo
            shape.y--;
          }
          shape.y++; // undo last fall (we're on the ground)
          shape.solidify();
          highest = max(highest, shape.y + shape.info.height + 1);
          if (!(i % (movements.size() * 5) == 0)) [[likely]]
            continue;
          if (i > movements.size() * 5) {
            uint pi = i / (movements.size() * 5);
            size_t diff = highest - prev;
            cout << "pi: " << pi << " diff: " << diff << '\r';
            pattern_detector.push_back(diff);
            if (pattern_detector.size() % 2 != 0 ||
                pattern_detector.size() <= 2)
              goto end;
            bool found = true;
            for (size_t i = 0; i < pattern_detector.size() / 2; i++)
              if (pattern_detector[i] !=
                  pattern_detector[i + pattern_detector.size() / 2]) {
                found = false;
                break;
              }
            if (found) {
              cout << "Pattern found at " << i << " (pi: " << pi << ")" << endl;

              vector<size_t> pattern;
              for (size_t i = 0; i < pattern_detector.size() / 2; i++)
                pattern.push_back(pattern_detector[i]);

              size_t remaining = rocks_to_simulate - i;
              size_t pattern_length = pattern.size();
              size_t pattern_misses = remaining % pattern_length;
              size_t pattern_offset = remaining / pattern_length;
              size_t pattern_sum = 0;
              for (size_t i = 0; i < pattern_length; i++)
                pattern_sum += pattern[i];
              size_t before = highest;
              highest += pattern_sum * pattern_offset / movements.size() / 5;
              size_t diff = highest - before;
              remaining -= pattern_length * pattern_offset;
              i += pattern_length * pattern_offset;
              // simulate a couple of rocks 💀💀💀💀
              for (; i <= rocks_to_simulate; i++) {
                ::shape{grid, shapes[i % 5],
                        pos{2, int64_t(highest - diff + 3)}};
                while (!shape.is_colliding()) {
                  auto move = movements[curr_move++];
                  curr_move %= movements.size();
                  shape.x += move == RIGHT ? 1 : -1;
                  if (shape.is_colliding())
                    shape.x += move == RIGHT ? -1 : 1; // undo
                  shape.y--;
                }
                shape.y++; // undo last fall (we're on the ground)
                shape.solidify();
                highest = max(highest, diff + shape.y + shape.info.height + 1);
                cout << "simulated " << i << " rocks" << '\r';
              }
              cout << endl << "Highest: " << highest << endl;
              cout << "there is a little bug somewhere but your answer is "
                      "probably somewhere around this. todo fix"
                   << endl;
              return highest;
            }
          }
        end:
          prev = highest;
        }
        return highest;
      });
}