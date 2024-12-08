#include <map>
#include <set>
#include <unordered_set>
#include "shared.h"

struct Map {
	std::array<std::vector<pos_t<>>, 255> antennas;
	std::ptrdiff_t size;
};

int main() {
	day(
		[](std::string_view inview) {
			std::string input{inview};
			Map map{};
			map.size = input.find('\n');
			std::erase(input, '\n');
			for (size_t i = 0; i != input.size(); ++i) {
				const char frequency = input[i];
				if (frequency != '.') {
					map.antennas[frequency].push_back(pos_t<>::from_1d(map.size, i));
				}
			}
			return map;
		},
		[](Map &map) {
			std::vector antinodes(static_cast<size_t>(map.size * map.size), '\0');
			for (const auto &[antennaI, antennas] : map.antennas | std::views::enumerate) {
				const char antennaChar = antennaI;
				for (auto &pos1 : antennas) {
					for (auto &pos2 : antennas) {
						if (&pos1 == &pos2)
							continue;
						const auto diff = pos2 - pos1;
						const auto one = pos1 - diff;
						const auto two = pos2 + diff;
						if (one.x >= 0 && one.y >= 0 && one.x < map.size && one.y < map.size)
							antinodes[one.to_1d(map.size)] = antennaChar;
						if (two.x >= 0 && two.y >= 0 && two.x < map.size && two.y < map.size)
							antinodes[two.to_1d(map.size)] = antennaChar;
					}
				}
			}
			// for (const auto& chunk : antinodes | std::views::chunk(map.size)) {
			// 	for (const char c : chunk)
			// 		std::cout << (c ? c : '.');
			// 	std::cout << '\n';
			// }
			size_t total = 0;
			for (char c : antinodes)
				if (c != '\0')
					++total;
			return total;
		},
		[](Map &map) {
			std::vector antinodes(static_cast<size_t>(map.size * map.size), '\0');
			for (const auto &[antennaI, antennas] : map.antennas | std::views::enumerate) {
				const char antennaChar = antennaI;
				for (auto &pos1 : antennas) {
					for (auto &pos2 : antennas) {
						if (&pos1 == &pos2)
							continue;
						const auto diff = pos2 - pos1;
						auto back = pos1;
						while (back.x >= 0 && back.y >= 0 && back.x < map.size && back.y < map.size) {
							antinodes[back.to_1d(map.size)] = antennaChar;
							back -= diff;
						}
						auto forward = pos2;
						while (forward.x >= 0 && forward.y >= 0 && forward.x < map.size && forward.y < map.size) {
							antinodes[forward.to_1d(map.size)] = antennaChar;
							forward += diff;
						}
					}
				}
			}
			// for (const auto& chunk : antinodes | std::views::chunk(map.size)) {
			// 	for (const char c : chunk)
			// 		std::cout << (c ? c : '.');
			// 	std::cout << '\n';
			// }
			size_t total = 0;
			for (const char c : antinodes)
				if (c != '\0')
					++total;
			return total;
		});
}
