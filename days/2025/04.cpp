#include "shared.h"

// 2d array, true if full
using Input = std::tuple<std::vector<bool>, int, int>;
static Input input(std::string_view input) {
	Input result{};
	auto &[grid, width, height] = result;
	width = input.find('\n');
	height = (input.size() + 1) / (width + 1);
	for (char c : input) {
		if (c == '@')
			grid.push_back(true);
		else if (c == '.')
			grid.push_back(false);
	}
	return result;
}

static int part_1(const Input &input) {
	auto &[grid, width, height] = input;

	int total = 0;
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			if (!grid[y * width + x])
				continue;
			int neighbours_including_self = 0;
			for (int j = std::max(0, y - 1); j <= std::min(height - 1, y + 1); j++)
				for (int i = std::max(0, x - 1); i <= std::min(width - 1, x + 1); i++)
					neighbours_including_self += grid[j * width + i];
			if (neighbours_including_self < 5)
				total++;
		}
	return total;
}

static int part_2(const Input &input) {
	// intentional copy so that the grid can be modified
	auto [grid, width, height] = input;

	int total = 0;
	int removed_just_now = 0;
	do {
		removed_just_now = 0;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++) {
				if (!grid[y * width + x])
					continue;
				int neighbours_including_self = 0;
				for (int j = std::max(0, y - 1); j <= std::min(height - 1, y + 1); j++)
					for (int i = std::max(0, x - 1); i <= std::min(width - 1, x + 1); i++) 
						neighbours_including_self += grid[j * width + i];
				if (neighbours_including_self < 5) {
					removed_just_now++;
					grid[y * width + x] = false;
				}
			}
		total += removed_just_now;
	} while (removed_just_now != 0);
	return total;
}

#include "main.h"
