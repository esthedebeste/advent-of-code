#include "shared.h"
#include <array>
#include <unordered_set>

using namespace std;

struct pos3d {
  int x, y, z;
  int manhattan_dist(pos3d other) const {
    return abs(x - other.x) + abs(y - other.y) + abs(z - other.z);
  }
  array<pos3d, 6> neighbours() const {
    return {pos3d{x + 1, y, z}, pos3d{x - 1, y, z}, pos3d{x, y + 1, z},
            pos3d{x, y - 1, z}, pos3d{x, y, z + 1}, pos3d{x, y, z - 1}};
  }
};
bool operator==(pos3d a, pos3d b) {
  return a.x == b.x && a.y == b.y && a.z == b.z;
}
namespace std {
template <> struct hash<pos3d> {
  size_t operator()(const pos3d &p) const {
    return hash<int>()(p.x) ^ hash<int>()(p.y) ^ hash<int>()(p.z);
  }
};
}

using input = unordered_set<pos3d>;

int main() {
  int minx = numeric_limits<int>::max(), miny = numeric_limits<int>::max(),
      minz = numeric_limits<int>::max(), maxx = numeric_limits<int>::min(),
      maxy = numeric_limits<int>::min(), maxz = numeric_limits<int>::min();
  day(
      [&](istream &input) {
        vector<vector<int>> parsed = lines(split_by(",", parse<int>))(input);
        unordered_set<pos3d> positions;
        for (auto vec : parsed) {
          int x = vec[0], y = vec[1], z = vec[2];
          positions.insert(pos3d{x, y, z});
          minx = min(minx, x), miny = min(miny, y), minz = min(minz, z);
          maxx = max(maxx, x), maxy = max(maxy, y), maxz = max(maxz, z);
        }
        return positions;
      },
      [](input positions) {
        int amount = 6 * positions.size();
        for (auto pos : positions)
          for (auto neighbour : pos.neighbours())
            if (positions.contains(neighbour))
              amount--;
        return amount;
      },
      [&](input positions) {
        // pretend i'm steam and i'm trying to get into the droplet frfr
        vector<pos3d> to_visit{{minx, miny, minz}};
        unordered_set<pos3d> visited;
        int amount = 0;
        while (!to_visit.empty()) {
          pos3d pos = to_visit.back();
          to_visit.pop_back();
          for (auto neighbour : pos.neighbours()) {
            if (visited.contains(neighbour) || neighbour.x < minx - 1 ||
                neighbour.x > maxx + 1 || neighbour.y < miny - 1 ||
                neighbour.y > maxy + 1 || neighbour.z < minz - 1 ||
                neighbour.z > maxz + 1)
              continue;
            if (positions.contains(neighbour)) {
              amount++;
              continue;
            }
            visited.insert(neighbour);
            to_visit.push_back(neighbour);
          }
        }
        return amount;
      });
}