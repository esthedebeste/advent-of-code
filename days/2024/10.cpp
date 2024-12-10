#define default_pos_t int16_t
#include <set>
#include "shared.h"

using dpt = default_pos_t;
int main() {
	day(
		[](std::string_view inview) {
			std::string input{inview};
			dpt size = input.find('\n');
			std::erase(input, '\n');
			return std::make_pair(input, size);
		},
		[](std::pair<std::string, dpt> &input) {
			auto &[field, size] = input;
			dpt total = 0;
			for (dpt y = 0; y < size; y++) {
				for (dpt x = 0; x < size; x++) {
					pos_t trailhead{x, y};
					if (field[trailhead.to_1d(size)] != '0')
						continue; // not a trailhead
					std::set<pos_t<>> nines{};
					dfs<pos_t<>>(
						trailhead,
						[&](const pos_t<> &pos) {
							if (field[pos.to_1d(size)] == '9') {
								nines.emplace(pos);
								return true;
							}
							return false;
						},
						[](const pos_t<> &pos) { return pos.neighbours(); },
						[&](const pos_t<> &prev, const pos_t<> &neighbour) {
							if (neighbour.x < 0 || neighbour.y < 0 || neighbour.x >= size || neighbour.y >= size)
								return false;
							if (field[neighbour.to_1d(size)] != field[prev.to_1d(size)] + 1)
								return false;
							return true;
						});
					total += nines.size();
				}
			}
			return total;
		},
		[](std::pair<std::string, dpt> &input) {
			auto &[field, size] = input;
			dpt total = 0;
			for (dpt y = 0; y < size; y++) {
				for (dpt x = 0; x < size; x++) {
					pos_t trailhead{x, y};
					if (field[trailhead.to_1d(size)] != '0')
						continue; // not a trailhead
					dfs<pos_t<>>(
						trailhead,
						[&](const pos_t<> &pos) {
							if (field[pos.to_1d(size)] == '9') {
								total++;
								return true;
							}
							return false;
						},
						[](const pos_t<> &pos) { return pos.neighbours(); },
						[&](const pos_t<> &prev, const pos_t<> &neighbour) {
							if (neighbour.x < 0 || neighbour.y < 0 || neighbour.x >= size || neighbour.y >= size)
								return false;
							if (field[neighbour.to_1d(size)] != field[prev.to_1d(size)] + 1)
								return false;
							return true;
						});
				}
			}
			return total;
		});
}
