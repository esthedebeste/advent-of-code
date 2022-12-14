#include "shared.h"
#include <unordered_set>

using namespace std;

struct pos {
  int x, y;
  pos down() const { return {x, y + 1}; }
  pos left() const { return {x - 1, y}; }
  pos right() const { return {x + 1, y}; }
  friend bool operator==(pos a, pos b) { return a.x == b.x && a.y == b.y; }
};
namespace std {
template <> struct hash<pos> {
  size_t operator()(const pos &x) const {
    return hash<int>()(x.x) ^ hash<int>()(x.y);
  }
};
}
using input = unordered_set<pos>;
constexpr pos drop_pos{500, 0};

int main() {
  int minx = numeric_limits<int>::max(), maxx = 0, maxy = 0;
  day(
      [&](ifstream &input) {
        auto parser = lines(
            split_by(" -> ", pair_transform(',', parse<decltype(pos::x)>)));
        vector<vector<pair<int, int>>> lines = parser(input);
        unordered_set<pos> solid;
        for (auto formation : lines) {
          for (size_t i = 1; i < formation.size(); i++) {
            auto [x1, y1] = formation[i - 1];
            auto [x2, y2] = formation[i];
            for (int x = min(x1, x2); x <= max(x1, x2); x++)
              for (int y = min(y1, y2); y <= max(y1, y2); y++)
                solid.insert({x, y});
            minx = min(minx, min(x1, x2));
            maxx = max(maxx, max(x1, x2));
            maxy = max(maxy, max(y1, y2));
          }
        }
        return solid;
      },
      [&](input solid) {
        int amount = 0;
        while (true) {
          pos sand = drop_pos;
          while (true) {
            if (sand.x < minx || sand.x > maxx || sand.y > maxy)
              return amount; // out of bounds
            if (!solid.contains(sand.down()))
              sand = sand.down();
            else if (!solid.contains(sand.down().left()))
              sand = sand.down().left();
            else if (!solid.contains(sand.down().right()))
              sand = sand.down().right();
            else
              break;
          }
          amount++;
          solid.insert(sand);
        }
        return -1;
      },
      [&](input solid) {
        auto is_solid = [&](pos p) {
          return p.y >= maxy + 2 || solid.contains(p);
        };
        int amount = 0;
        while (true) {
          pos sand = drop_pos;
          while (true) {
            if (!is_solid(sand.down()))
              sand = sand.down();
            else if (!is_solid(sand.down().left()))
              sand = sand.down().left();
            else if (!is_solid(sand.down().right()))
              sand = sand.down().right();
            else
              break;
          }
          amount++;
          if (sand == drop_pos)
            return amount;
          solid.insert(sand);
        }
        return -1;
      });
}