#include "shared.h"
#include <map>

using namespace std;

struct Point {
  int x, y;
};
bool operator<(const Point &l, const Point &r) {
  return l.x < r.x || l.x == r.x && l.y < r.y;
}
struct Vent {
  Point a, b;
};

int main() {
  map<Point, int> points;
  day(
      [](ifstream &file) {
        vector<Vent> vents;
        while (file.peek() != EOF) {
          int x1, y1, x2, y2;
          file >> x1;
          file.ignore(1); // ,
          file >> y1;
          file.ignore(4); // " -> "
          file >> x2;
          file.ignore(1); // ,
          file >> y2;
          file.ignore(1); // newline
          vents.push_back({{x1, y1}, {x2, y2}});
        }
        return vents;
      },
      [&](vector<Vent> vents) {
        for (auto vent : vents)
          if (vent.a.x == vent.b.x || vent.a.y == vent.b.y)
            for (int x = min(vent.a.x, vent.b.x); x <= max(vent.a.x, vent.b.x);
                 x++)
              for (int y = min(vent.a.y, vent.b.y);
                   y <= max(vent.a.y, vent.b.y); y++)
                points[{x, y}]++;
        int overlapc = 0;
        for (auto &[_, v] : points)
          if (v >= 2)
            overlapc++;
        return overlapc;
      },
      [&](vector<Vent> vents) {
        for (auto [a, b] : vents) {
          if (a.x == b.x || a.y == b.y)
            continue;
          int dx = a.x > b.x ? -1 : 1;
          int dy = a.y > b.y ? -1 : 1;
          for (int x = a.x, y = a.y;; x += dx, y += dy) {
            points[{x, y}]++;
            if (x == b.x && y == b.y)
              break;
          }
        }

        int overlapc = 0;
        for (auto &[_, v] : points)
          if (v >= 2)
            overlapc++;
        return overlapc;
      });
}