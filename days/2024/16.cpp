#define default_pos_t int
#include <set>
#include "shared.h"

using dpt = pos_t<>::member_type;

struct Map {
	std::string field;
	dpt size;
	pos_t<> start;
	pos_t<> end;
	char &at(const pos_t<> &pos) { return field[pos.to_1d(size)]; }
	char &operator[](const pos_t<> &pos) { return at(pos); }
	const char &at(const pos_t<> &pos) const { return field[pos.to_1d(size)]; }
	const char &operator[](const pos_t<> &pos) const { return at(pos); }
};

int main() {
	day(
		[](std::string_view inview) {
			std::string input{inview};
			Map map{};
			map.size = input.find('\n');
			std::erase(input, '\n');
			const size_t start_i = input.find('S');
			input[start_i] = '.';
			map.start = pos_t<>::from_1d(map.size, start_i);
			const size_t end_i = input.find('E');
			input[end_i] = '.';
			map.end = pos_t<>::from_1d(map.size, end_i);
			map.field = input;
			return map;
		},
		[](const Map &map) {
			return dijkstra<std::pair<pos_t<>, pos_t<>>>(
				std::make_pair(map.start, pos_t{1, 0}), [&](const std::pair<pos_t<>, pos_t<>> &pair) { return pair.first == map.end; },
				[](const std::pair<pos_t<>, pos_t<>> &pair) -> std::array<std::pair<pos_t<>, pos_t<>>, 3> {
					const auto &[pos, dir] = pair;
					auto l = dir.rotate_left_90(), r = dir.rotate_right_90();
					return {std::pair{pos + dir, dir}, std::pair{pos + r, r}, std::pair{pos + l, l}};
				},
				[&](const std::pair<pos_t<>, pos_t<>> &_prev, const std::pair<pos_t<>, pos_t<>> &pair) {
					const auto &[pos, dir] = pair;
					return map[pos] == '.';
				},
				[&](const std::pair<pos_t<>, pos_t<>> &prev, const std::pair<pos_t<>, pos_t<>> &next) {
					return (prev.second != next.second) * 1000 + 1;
				});
		},
		[](const Map &map) {
			// write out dijkstra so we can have a chain with multiple prevs
			using elem = std::pair<int, std::pair<pos_t<>, pos_t<>>>;
			std::priority_queue<elem, std::vector<elem>, std::greater<elem>> queue;
			std::unordered_map<std::pair<pos_t<>, pos_t<>>, std::set<std::pair<pos_t<>, pos_t<>>>> chain;
			std::unordered_map<std::pair<pos_t<>, pos_t<>>, int> cost_so_far;

			const auto start = std::make_pair(map.start, pos_t{1, 0});
			queue.emplace(0, start);
			chain[start] = {start};
			cost_so_far[start] = 0;

			while (!queue.empty()) {
				auto current = queue.top().second;
				auto &[pos, dir] = current;
				queue.pop();

				if (pos == map.end) {
					break;
				}

				auto l = dir.rotate_left_90(), r = dir.rotate_right_90();
				for (const auto &[next, cost] :
						 {std::pair{std::pair{pos + dir, dir}, 1}, {std::pair{pos + r, r}, 1001}, {std::pair{pos + l, l}, 1001}}) {
					if (map[next.first] != '.')
						continue;
					int new_cost = cost_so_far[current] + cost;
					if (!cost_so_far.contains(next) || new_cost <= cost_so_far[next]) {
						cost_so_far[next] = new_cost;
						auto &cn = chain[next];
						if (new_cost < cost_so_far[next])
							cn.clear();
						cn.insert(current);
						queue.emplace(new_cost, next);
					}
				}
			}
			std::set<pos_t<>> visited;
			std::set<std::pair<pos_t<>, pos_t<>>> visited_pairs;
			std::vector<std::pair<pos_t<>, pos_t<>>> walk{{map.end, pos_t{1, 0}}, {map.end, {0, -1}}, {map.end, {-1, 0}}, {map.end, {0, 1}}};
			while (!walk.empty()) {
				auto c = walk.back();
				walk.pop_back();
				if (visited_pairs.contains(c))
					continue;
				walk.append_range(chain[c]);
				visited_pairs.insert(c);
				visited.emplace(c.first);
			}
			return visited.size();
		});
}
