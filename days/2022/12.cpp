#include "shared.h"
#include <algorithm>
#include <queue>
#include <unordered_map>

using namespace std;

using field = vector<string>;
pos dirs[4]{pos{1, 0}, pos{-1, 0}, pos{0, 1}, pos{0, -1}};

vector<pos> neighbors(field &grid, pos from) {
  vector<pos> results;
  for (pos dir : dirs) {
    pos next{from.x + dir.x, from.y + dir.y};
    if (next.x > 0 && next.x < grid.size() && next.y > 0 &&
        next.y < grid[0].size() &&
        grid[next.x][next.y] - 1 <= grid[from.x][from.y])
      results.push_back(next);
  }
  return results;
}

int main() {
  pos start, goal;
  day(
      lines(),
      [&](field &input) {
        for (int i = 0; i < input.size(); i++)
          for (int j = 0; j < input[i].size(); j++)
            if (input[i][j] == 'S') {
              start = {i, j};
              input[i][j] = 'a';
            } else if (input[i][j] == 'E') {
              goal = {i, j};
              input[i][j] = 'z';
            }
        return dijkstra(start, goal,
                        [&](pos p) { return neighbors(input, p); });
      },
      [&](field &input) {
        int shortest = numeric_limits<int>::max();
        for (int i = 0; i < input.size(); i++)
          for (int j = 0; j < input[i].size(); j++)
            if (input[i][j] == 'a') {
              auto path = dijkstra(pos{i, j}, goal,
                                   [&](pos p) { return neighbors(input, p); });
              if (path == -1)
                continue;
              shortest = min(shortest, path);
            }
        return shortest;
      });
}