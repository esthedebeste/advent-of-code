#include <set>
#include <unordered_map>
#include <unordered_set>
#include "shared.h"

struct Input {
	std::vector<pos> original, unique;
	std::vector<int> x, y;
};

static size_t index_of(const std::vector<int> &v, int x, size_t from, size_t to) {
	if (from == to)
		if (v[from] == x)
			return from;
		else
			return -1;
	size_t center = (from + to) / 2;
	int cv = v[center];
	if (cv > x)
		return index_of(v, x, from, center - 1);
	if (cv < x)
		return index_of(v, x, center + 1, to);
	// if(cv == x)
	return center;
}
static size_t index_of(const std::vector<int> &v, int x) { return index_of(v, x, 0, v.size() - 1); }

static Input input(std::string_view input) {
	Input result{};
	result.original = lines([](std::string_view line) {
		size_t comma = line.find(',');
		auto x = line.substr(0, comma), y = line.substr(comma + 1);
		return pos{parse_num(x), parse_num(y)};
	})(input);
	std::set<int> x_set{}, y_set{};
	for (pos &p : result.original) x_set.insert(p.x), y_set.insert(p.y);
	result.x = {x_set.begin(), x_set.end()};
	result.y = {y_set.begin(), y_set.end()};
	assert(std::ranges::is_sorted(result.x) && std::ranges::is_sorted(result.y));
	for (pos &p : result.original) result.unique.emplace_back(index_of(result.x, p.x), index_of(result.y, p.y));
	return result;
}

static int64_t part_1(const Input &input) {
	const auto &original = input.original;
	int64_t max = 0;
	for (auto a = original.begin(); a != original.end(); ++a) {
		for (auto b = a + 1; b != original.end(); ++b) {
			int64_t size = static_cast<int64_t>(std::abs(a->x - b->x) + 1) * (std::abs(a->y - b->y) + 1);
			max = std::max(max, size);
		}
	}
	return max;
}

static bool is_inside(pos p, std::unordered_set<pos> &outline) {
	int c = 0;
	for (; p.x >= -1; --p.x) {
		if (outline.contains(p))
			++c;
	}
	return c % 2 == 1; // even = outside
}

static int64_t part_2(const Input &input) {
	auto &original = input.original, &unique = input.unique;
	const int unique_x_size = input.x.size(), unique_y_size = input.y.size();
	std::vector<bool> outline(static_cast<size_t>(unique_x_size * unique_y_size), false);
	for (size_t i = 0; i < unique.size(); ++i) {
		auto a = unique[i], b = unique[(i + 1) % unique.size()];
		pos direction = b - a;
		direction = {static_cast<int>(direction.x < 0			 ? -1
																		: direction.x == 0 ? 0
																											 : 1),
								 static_cast<int>(direction.y < 0			 ? -1
																		: direction.y == 0 ? 0
																											 : 1)};
		for (; a != b; a += direction) {
			outline[a.to_1d(unique_x_size)] = true;
		}
		outline[a.to_1d(unique_x_size)] = true;
	}
	std::vector<bool> outside_neg1(static_cast<size_t>((unique_x_size + 2) * (unique_y_size + 2)), false);
	{
		// flood fill
		std::vector<pos> stack{};
		stack.emplace_back(unique_x_size, unique_y_size);
		while (!stack.empty()) {
			pos curr = stack.back();
			stack.pop_back();
			if (curr.x >= 0 && curr.x < unique_x_size && curr.y >= 0 && curr.y < unique_y_size && outline[curr.to_1d(unique_x_size)])
				continue;
			auto curr_i = (curr + pos{1, 1}).to_1d(unique_x_size + 2);
			if (outside_neg1[curr_i])
				continue;
			outside_neg1[curr_i] = true;
			for (pos p : curr.neighbours())
				if (p.x >= -1 && p.x <= unique_x_size && p.y >= -1 && p.y <= unique_y_size)
					stack.push_back(p);
		}
	}
#ifndef NDEBUG
	// visual representation of the shape
	for (int y = -1; y <= unique_y_size; ++y) {
		for (int x = -1; x <= unique_x_size; ++x) {
			std::clog << (outside_neg1[(pos{x + 1, y + 1}).to_1d(input.x.size() + 2)] ? '#' : '.');
		}
		std::clog << '\n';
	}
	std::clog << std::endl;
#endif

	int64_t max = 0;
	auto zipped = std::views::zip(original, unique);
	for (auto ita = zipped.begin(); ita != zipped.end(); ++ita) {
		for (auto itb = ita + 1; itb != zipped.end(); ++itb) {
			auto [original_a, a] = *ita;
			auto [original_b, b] = *itb;
			int64_t size = static_cast<int64_t>(std::abs(original_a.x - original_b.x) + 1) * (std::abs(original_a.y - original_b.y) + 1);
			if (size > max) {
				pos top_left{std::min(a.x, b.x), std::min(a.y, b.y)};
				pos top_right{std::max(a.x, b.x), std::min(a.y, b.y)};
				pos bottom_left{std::min(a.x, b.x), std::max(a.y, b.y)};
				pos bottom_right{std::max(a.x, b.x), std::max(a.y, b.y)};
				for (pos p = top_left; p != top_right; p.x++)
					if (outside_neg1[(p + pos{1, 1}).to_1d(unique_x_size + 2)])
						goto next;
				for (pos p = top_right; p != bottom_right; p.y++)
					if (outside_neg1[(p + pos{1, 1}).to_1d(unique_x_size + 2)])
						goto next;
				for (pos p = bottom_right; p != bottom_left; p.x--)
					if (outside_neg1[(p + pos{1, 1}).to_1d(unique_x_size + 2)])
						goto next;
				for (pos p = bottom_left; p != top_left; p.y--)
					if (outside_neg1[(p + pos{1, 1}).to_1d(unique_x_size + 2)])
						goto next;
				max = size;
			}
		next:;
		}
	}
	return max;
}

#include "main.h"
