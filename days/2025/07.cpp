#include <set>
#include "shared.h"

struct Input {
	std::string_view input;
	pos start;
	int width, height;
	const char &at(int x, int y) const { return input[x + y * (width + 1)]; }
};

static Input input(std::string_view input) {
	Input result{};
	result.input = input;
	result.width = input.find('\n');
	int start_i = input.find('S');
	result.start = {start_i % (result.width + 1), start_i / (result.width + 1)};
	result.height = (input.size() + 1) / (result.width + 1);
	return result;
}

static int part_1(const Input &input) {
	auto can_get_here = std::vector<int8_t>(input.width * input.height, false);
	can_get_here[input.start.x + input.start.y * input.width] = 1;
	int splits = 0;
	for (int y = 1; y < input.height; y++) {
		for (int x = 0; x < input.width; x++) {
			int i = y * input.width + x;
			auto &reachable = can_get_here[i];
			char map_value = input.at(x, y);
			// from above
			int above_i = (y - 1) * input.width + x;
			if (input.at(x, y - 1) != '^')
				reachable = reachable | can_get_here[above_i];
			if (reachable && map_value == '^') {
				splits++;
				can_get_here[i - 1] = 1;
				can_get_here[i + 1] = 1;
			}
		}
	}
	return splits;
}

static uint64_t part_2(const Input &input) {
	auto possible_ways_to_get_here = std::vector<uint64_t>(input.width * input.height, 0);
	possible_ways_to_get_here[input.start.x + input.start.y * input.width] = 1;
	for (int y = 1; y < input.height; y++) {
		for (int x = 0; x < input.width; x++) {
			int i = y * input.width + x;
			auto &possibilities = possible_ways_to_get_here[i];
			char map_value = input.at(x, y);
			// from above
			int above_i = (y - 1) * input.width + x;
			if (input.at(x, y - 1) != '^')
				possibilities += possible_ways_to_get_here[above_i];
			if (map_value == '^') {
				possible_ways_to_get_here[i - 1] += possibilities;
				possible_ways_to_get_here[i + 1] += possibilities;
			}
		}
	}
	uint64_t timelines = 0;
	for (int i = (input.height - 1) * input.width; i < input.height * input.width; i++) {
		timelines += possible_ways_to_get_here[i];
	}
	return timelines;
}

#include "main.h"
