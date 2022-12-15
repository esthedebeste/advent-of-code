#define default_pos_t int64_t
#include "shared.h"

using namespace std;
using i64 = int64_t;
struct sensor : public pos {
  pos beacon;
  i64 range;
  sensor(pos self, pos beacon) : pos{self}, beacon{beacon} {
    range = manhattan_distance(beacon);
  }
};

using input = vector<sensor>;

int main() {
  i64 minx = numeric_limits<i64>::max(), maxx = 0;
  day(
      lines([&](ifstream &input) {
        int sx, sy, bx, by;
        input >> "Sensor at x=" >> sx >> ", y=" >> sy >>
            ": closest beacon is at x=" >> bx >> ", y=" >> by;
        sensor s{{sx, sy}, {bx, by}};
        minx = min(minx, sx - s.range);
        maxx = max(maxx, sx + s.range);
        return sensor{{sx, sy}, {bx, by}};
      }),
      [&](input sensors) {
        constexpr auto Y = 2'000'000;
        int amount = 0;
        for (i64 x = minx; x <= maxx; x++) {
          pos p{x, Y};
          for (auto sensor : sensors)
            if (sensor.beacon == p)
              break;
            else if (sensor.manhattan_distance(p) <= sensor.range) {
              amount++;
              break;
            }
        }
        return amount;
      },
      [&](input sensors) -> i64 {
        constexpr auto MAX = 4'000'000;
        // move along edges of sensors
        for (auto &sensor : sensors) {
          // start at the bottom corner
          pos pos{sensor.x, sensor.y + sensor.range + 1};
          for (int i = 0; i < 4; i++)
            for (int j = 0; j < sensor.range + 1; j++) {
              if (pos.x >= 0 && pos.y >= 0 && pos.x <= MAX && pos.y <= MAX)
                  [[likely]] {
                for (auto &s : sensors)
                  if (s.manhattan_distance(pos) <= s.range)
                    goto move;
                return pos.x * 4'000'000 + pos.y; // found it!
              }

            move:
              switch (i) {
              case 0:
                pos = pos.right().up(); // go towards the right corner
                break;
              case 1:
                pos = pos.left().up(); // go towards the top corner
                break;
              case 2:
                pos = pos.left().down(); // go towards the left corner
                break;
              case 3:
                pos = pos.right().down(); // go towards the bottom corner
                break;
              default:
                unreachable();
              }
            }
        }
        throw runtime_error("no solution found");
      });
}