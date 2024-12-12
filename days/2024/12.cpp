#include "shared.h"

using dpt = pos_t<>::member_type;
void print_perimeter(const dpt size, const std::string &field, const std::unordered_map<std::pair<pos_t<>, pos_t<>>, bool>& perimeter) {
	for (dpt y = -1; y < size + 1; ++y) {
		for (dpt x = -1; x < size + 1; ++x) {
			pos_t pos{x, y};
			const bool horizontal = perimeter.contains({pos, {0, 1}}) || perimeter.contains({pos, {0, -1}});
			const bool vertical = perimeter.contains({pos, {1, 0}}) || perimeter.contains({pos, {-1, 0}});
			const bool within_bounds = x >= 0 && y >= 0 && y < size && x < size;
			if (horizontal && vertical)
				std::cout << '+';
			else if (horizontal)
				std::cout << '-';
			else if (vertical)
				std::cout << '|';
			else if (within_bounds)
				std::cout << field[pos.to_1d(size)];
			else
				std::cout << ' ';
		}
		std::cout << '\n';
	}
	std::cout << std::flush;
}

int main() {
	day(
		[](const std::string_view input_view) {
			std::string input{input_view};
			dpt size = input.find('\n');
			std::erase(input, '\n');
			return std::make_pair(input, size);
		},
		[](const std::pair<std::string, dpt> &input) {
			const auto &[field, size] = input;
			std::vector visited(field.size(), false);
			size_t total = 0;
			for (dpt y = 0; y < size; ++y) {
				for (dpt x = 0; x < size; ++x) {
					size_t area = 0;
					size_t perimeter = 0;
					pos_t start{x, y};
					const dpt start_i = start.to_1d(size);
					if (visited[start_i])
						continue;
					visited[start_i] = true;
					const char searching = field[start_i];
					std::vector queue(1, start);
					while (!queue.empty()) {
						pos_t pos = queue.back();
						queue.pop_back();
						area++;
						for (const auto &neighbour : pos.neighbours()) {
							if (neighbour.x < 0 || neighbour.y < 0 || neighbour.x >= size || neighbour.y >= size) {
								perimeter++;
								continue;
							}
							const auto neighbour_i = neighbour.to_1d(size);
							if (field[neighbour_i] != searching) {
								perimeter++;
								continue;
							}
							if (!visited[neighbour_i]) {
								queue.push_back(neighbour);
								visited[neighbour_i] = true;
							}
						}
					}
					total += area * perimeter;
				}
			}
			return total;
		},
		[](const std::pair<std::string, dpt> &input) {
			const auto &[field, size] = input;
			std::vector visited(field.size(), false);
			size_t total = 0;
			for (dpt y = 0; y < size; ++y) {
				for (dpt x = 0; x < size; ++x) {
					size_t area = 0;
					std::unordered_map<std::pair<pos_t<>, pos_t<>>, bool> perimeter{};
					pos_t start{x, y};
					const dpt start_i = start.to_1d(size);
					if (visited[start_i])
						continue;
					visited[start_i] = true;
					const char searching = field[start_i];
					std::vector queue(1, start);
					while (!queue.empty()) {
						pos_t pos = queue.back();
						queue.pop_back();
						area++;
						for (const auto &neighbour : pos.neighbours()) {
							auto dir = neighbour - pos;
							const auto neighbour_i = neighbour.to_1d(size);
							if (neighbour.x < 0 || neighbour.y < 0 || neighbour.x >= size || neighbour.y >= size || field[neighbour_i] != searching) {
								perimeter.emplace(std::make_pair(neighbour, dir), false);
								continue;
							}
							if (!visited[neighbour_i]) {
								queue.push_back(neighbour);
								visited[neighbour_i] = true;
							}
						}
					}
					// print_perimeter(size, field, perimeter);
					size_t sides = 0;
					for (auto &[p, side_done] : perimeter) {
						if (side_done)
							continue;
						side_done = true;
						auto &[pos, dir] = p;
						auto side_dir = dir.rotate_right_90();
						side_dir.x = std::abs(side_dir.x);
						side_dir.y = std::abs(side_dir.y);
						auto up = pos + side_dir;
						auto found = perimeter.end();
						while ((found = perimeter.find(std::make_pair(up, dir))) != perimeter.end()) {
							found->second = true;
							up += side_dir;
						}
						auto down = pos - side_dir;
						while ((found = perimeter.find(std::make_pair(down, dir))) != perimeter.end()) {
							found->second = true;
							down -= side_dir;
						}
						sides++;
					}
					total += area * sides;
				}
			}
			return total;
		});
}
