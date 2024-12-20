#define default_pos_t int
#ifndef NDEBUG
#define DO_PRINTING
#endif
#include <set>
#include "shared.h"

using dpt = pos_t<>::member_type;

struct Map {
	std::vector<bool> field;
	dpt size;
	pos_t<> start;
	pos_t<> end;
	[[nodiscard]] std::vector<bool>::reference at(const pos_t<> &pos) { return field[pos.to_1d(size)]; }
	[[nodiscard]] std::vector<bool>::reference operator[](const pos_t<> &pos) { return at(pos); }
	[[nodiscard]] std::vector<bool>::const_reference at(const pos_t<> &pos) const { return field[pos.to_1d(size)]; }
	[[nodiscard]] std::vector<bool>::const_reference operator[](const pos_t<> &pos) const { return at(pos); }
};

void print_costs(const Map &map, const std::vector<int> &cost) {
#ifndef DO_PRINTING
	return;
#endif
	for (int y = 0; y < map.size; y++) {
		for (int x = 0; x < map.size; x++)
			std::cout << (map.field[y * map.size + x]								 ? "#####"
											: (map.start.y == y && map.start.x == x) ? " SSS "
											: (map.end.y == y && map.end.x == x)		 ? " EEE "
																															 : "     ");
		std::cout << '\n';
		for (int x = 0; x < map.size; x++) {
			if (map.field[y * map.size + x])
				std::cout << "#####";
			else
				std::cout << std::setfill(' ') << std::setw(4) << cost[y * map.size + x] << ' ';
		}
		std::cout << '\n';
		for (int x = 0; x < map.size; x++) std::cout << (map.field[y * map.size + x] ? "#####" : "     ");
		std::cout << '\n';
	}
}

int get_hundred_skips(const Map &map,
	const std::vector<int> &cost,
	const int picoseconds) {

	int hundreds = 0;
#ifdef DO_PRINTING
	std::vector saving(map.size * map.size, 0);
#endif
	for (const auto &[start_i, is_wall] : std::views::enumerate(map.field)) {
		if (is_wall)
			continue;
		for (const auto &[end_i, is_wall] : std::views::enumerate(map.field)) {
			if (is_wall || start_i == end_i)
				continue;
			const auto start = pos_t<>::from_1d(map.size, start_i);
			const auto end = pos_t<>::from_1d(map.size, end_i);
			// future: optimize so we dont iterate for manhattan > picoseconds
			const auto manhattan = start.manhattan_distance(end);
			if (manhattan > picoseconds)
				continue;
			const auto diff = cost[start_i] - cost[end_i];
			if (manhattan < diff) {
				// cool skip!
				const auto saved = diff - manhattan;
#ifdef DO_PRINTING
				saving[saved]++;
#endif
				if (saved >= 100)
					hundreds++;
			}
		}
	}
#ifdef DO_PRINTING
	for (const auto &[saving, amount] : std::views::enumerate(saving) | std::views::drop(50))
		if (amount > 1)
			std::cout << "There are " << amount << " cheats that save " << saving << " picoseconds." << std::endl;
		else if (amount == 1)
			std::cout << "There is one cheat that saves " << saving << " picoseconds." << std::endl;
#endif
	return hundreds;
}

int main() {
	std::vector<int> cost;
	day(
		[](std::string_view input) {
			Map map{};
			map.size = input.find('\n');
			map.start = pos_t<>::from_1d(map.size + 1, input.find('S'));
			map.end = pos_t<>::from_1d(map.size + 1, input.find('E'));
			map.field.reserve(map.size * map.size);
			for (const char c : input)
				if (c != '\n')
					map.field.push_back(c == '#');
			return map;
		},
		[&](const Map &map) -> dpt {
			cost = std::vector(map.field.size(), 0);
			{
				// find the distance between every track square and the end
				std::vector<pos_t<>> queue{};
				queue.emplace_back(map.end);
				while (!queue.empty()) {
					pos_t<> current = queue.back();
					queue.pop_back();
					for (const pos_t<> &next : current.neighbours()) {
						if (map[next])
							continue;
						const auto next_i = next.to_1d(map.size);
						if (const int new_cost = cost[current.to_1d(map.size)] + 1; cost[next_i] == 0 || new_cost < cost[next_i]) {
							cost[next_i] = new_cost;
							queue.emplace_back(next);
						}
					}
				}
				cost[map.end.to_1d(map.size)] = 0;
			}
			print_costs(map, cost);
			return get_hundred_skips(map, cost, 2);
		},
		[&](const Map &map) {
			return get_hundred_skips(map, cost, 20);
		});
}
