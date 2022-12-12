#include "shared.h"
#include <algorithm>
#include <queue>
#include <unordered_map>

using namespace std;

using field = vector<string>;
using pos = pair<int, int>;
pos dirs[4]{pos{1, 0}, pos{-1, 0}, pos{0, 1}, pos{0, -1}};

vector<pos> neighbors(field &grid, pos from) {
  vector<pos> results;
  for (pos dir : dirs) {
    pos next{from.first + dir.first, from.second + dir.second};
    if (next.first > 0 && next.first < grid.size() && next.second > 0 &&
        next.second < grid[0].size() &&
        grid[next.first][next.second] - 1 <= grid[from.first][from.second])
      results.push_back(next);
  }
  return results;
}

auto dijkstra(field &field, pos start, pos goal) {
  using elem = pair<int, pos>;
  priority_queue<elem, vector<elem>, greater<elem>> queue;
  unordered_map<pos, pos> chain;
  unordered_map<pos, int> cost_so_far;

  queue.emplace(0, start);
  chain[start] = start;
  cost_so_far[start] = 0;

  while (!queue.empty()) {
    pos current = queue.top().second;
    queue.pop();

    if (current == goal)
      break; // found!

    for (pos next : neighbors(field, current)) {
      int new_cost = cost_so_far[current] + 1;
      if (!cost_so_far.contains(next) || new_cost < cost_so_far[next]) {
        cost_so_far[next] = new_cost;
        chain[next] = current;
        queue.emplace(new_cost, next);
      }
    }
  }

  int len = 0;
  pos current = goal;
  if (!chain.contains(goal))
    return -1; // no path found :(
  while (current != start) {
    len++;
    current = chain[current];
  }
  return len;
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
        return dijkstra(input, start, goal);
      },
      [&](field &input) {
        int shortest = numeric_limits<int>::max();
        for (int i = 0; i < input.size(); i++)
          for (int j = 0; j < input[i].size(); j++)
            if (input[i][j] == 'a') {
              auto path = dijkstra(input, {i, j}, goal);
              if (path == -1)
                continue;
              shortest = min(shortest, path);
            }
        return shortest;
      });
}