#include <unordered_set>
#include <unordered_map>
#include "shared.h"

using Input = std::vector<pos>;

static Input input(std::string_view input) {
	return lines([](std::string_view line) {
		size_t comma = line.find(',');
		auto x = line.substr(0, comma), y = line.substr(comma + 1);
		return pos{parse_num(x), parse_num(y)};
	})(input);
}

static int64_t part_1(const Input &input) {
	int64_t max = 0;
	for (auto a = input.begin(); a != input.end(); ++a) {
		for (auto b = a + 1; b != input.end(); ++b) {
			int64_t size = static_cast<int64_t>(std::abs(a->x - b->x) + 1) * (std::abs(a->y - b->y) + 1);
			max = std::max(max, size);
		}
	}
	return max;
}

static bool is_inside(pos p, std::unordered_set<pos> &outline) {
	int c = 0;
	for (; p.x > -0; --p.x) {
		if (outline.contains(p))
			++c;
	}
	return c % 2 == 1; // even = outside
}

static int64_t part_2(const Input &input) {
	// geometry #crying
	std::unordered_set<pos> outline{};
	for (size_t i = 0; i < input.size(); ++i) {
		auto a = input[i], b = input[(i + 1) % input.size()];
		pos direction = b - a;
		direction = {static_cast<int>(direction.x < 0			 ? -1
																		: direction.x == 0 ? 0
																											 : 1),
								 static_cast<int>(direction.y < 0			 ? -1
																		: direction.y == 0 ? 0
																											 : 1)};
		for (; a != b; a += direction) {
			outline.insert(a);
		}
	}	


	int64_t max = 0;
	for (auto a = input.begin(); a != input.end(); ++a) {
		for (auto b = a + 1; b != input.end(); ++b) {
			int64_t size = static_cast<int64_t>(std::abs(a->x - b->x) + 1) * (std::abs(a->y - b->y) + 1);
			if (size > max) {
				pos top_left{std::min(a->x, b->x), std::min(a->x, b->x)};
				pos top_right{std::max(a->x, b->x), std::min(a->y, b->y)};
				pos bottom_left{std::max(a->x, b->x), std::max(a->y, b->y)};
				pos bottom_right{std::max(a->x, b->x), std::max(a->y, b->y)};
				pos p = top_left;
				for (pos p = top_left; p.x <= top_right.x; p.x++) 
						if (!is_inside(p, outline))
							goto next;
				for (pos p = top_right; p.y <= bottom_right.y; p.y++)
					if (!is_inside(p, outline))
						goto next;
				for (pos p = bottom_right; p.x >= bottom_left.x; p.x--)
					if (!is_inside(p, outline))
						goto next;
				for (pos p = bottom_left; p.y >= top_left.y; p.y--)
					if (!is_inside(p, outline))
						goto next;
				max = size;
			}
		next:;
		}
	}
	return max;
}

#include "main.h"
