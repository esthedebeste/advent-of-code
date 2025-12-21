#include <bitset>
#include "shared.h"

struct Shape : public std::bitset<9> {
	using std::bitset<9>::bitset;
	size_t area() const { return count(); }
};
struct Region {
	int8_t width, height;
	std::vector<int8_t> counts;
};
struct Input {
	std::vector<Shape> shapes;
	std::vector<Region> regions;
};

static Input input(std::string_view input) {
	Input result{};
	size_t end_present;
	while ((end_present = input.find("\n\n")) != std::string_view::npos) {
		std::string_view present = input.substr(0, end_present);
		input = input.substr(end_present + 2);
		present = present.substr(1); // skip number
		assert_check(present, ":");
		Shape &shape = result.shapes.emplace_back();
		for (int row = 0; row < 3; ++row) {
			assert_check(present, '\n');
			for (int col = 0; col < 3; ++col) {
				shape.set(row * 3 + col, present[0] == '#');
				present = present.substr(1);
			}
		}
	}
	result.regions = lines([](std::string_view line) {
		Region region{};
		region.width = parse_num_from<uint8_t>(line);
		assert_check(line, 'x');
		region.height = parse_num_from<uint8_t>(line);
		assert_check(line, ": ");
		do {
			region.counts.push_back(parse_num_from<uint8_t>(line));
		} while (check(line, ' '));
		return region;
	})(input);
	return result;
}

static int part_1(const Input &input) {
	int fits = 0;
	for (auto &region : input.regions) {
		int total_area = 0;
		for (const auto &[count, shape] : std::views::zip(region.counts, input.shapes)) {
			total_area += count * shape.area();
		}
		if (region.height * region.width >= total_area)
			++fits;
	}
	return fits;
}

#include "main.h"
