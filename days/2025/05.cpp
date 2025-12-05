#include "shared.h"

using Range = std::pair<uint64_t, uint64_t>;
using Input = std::pair<std::vector<Range>, std::vector<uint64_t>>;

static Input input(std::string_view input) {
	Input result{};
	auto &[ranges, ingredients] = result;

	size_t split_point = input.find("\n\n");
	auto ranges_str = input.substr(0, split_point);
	auto ingredients_str = input.substr(split_point + 2);
	ranges = lines([](std::string_view range) {
		size_t split_point = range.find('-');
		auto from_str = range.substr(0, split_point);
		auto to_str = range.substr(split_point + 1);
		return std::make_pair(parse_num<uint64_t>(from_str), parse_num<uint64_t>(to_str));
	})(ranges_str);
	ingredients = lines([](std::string_view ingredient) { return parse_num<uint64_t>(ingredient); })(ingredients_str);

	return result;
}

static bool in_range(uint64_t x, const Range &range) { return x >= range.first && x <= range.second; }

static uint64_t part_1(const Input &input) {
	auto &[ranges, ingredients] = input;
	uint64_t total = 0;
	for (auto i : ingredients) {
		for (auto &range : ranges) {
			if (in_range(i, range)) {
				total++;
				break;
			}
		}
	}
	return total;
}


// a MUST <= b.
static std::optional<Range> merge_ranges(const Range &a, const Range& b) { 
	if (in_range(b.first, a)) {
		return std::make_optional(std::make_pair(a.first, std::max(a.second, b.second)));
	}
	return std::nullopt;
}

static uint64_t part_2(const Input& input) {
	auto &[ranges, ingredients] = input;
	// sort all ranges by their first element, then their second.
	// use a linked list to cheaply remove middle elements
	std::list<Range> ranges_merged{ranges.begin(), ranges.end()};
	ranges_merged.sort();
	// merge them
	int merged_count = 0;
	do {
		merged_count = 0;
		auto prev = ranges_merged.begin();
		auto it = ranges_merged.begin();
		++it;
		while(it != ranges_merged.end()) {
			auto merged = merge_ranges(*prev, *it);
			if (merged) {
				++merged_count;
				*prev = *merged;
				it = ranges_merged.erase(it);
			} else {
				prev = it;
				++it;
			}
		}
	} while (merged_count > 0);
	uint64_t total = 0;
	for (auto &[start, end] : ranges_merged) {
		total += end - start + 1; // + 1 for inclusive
	}
	return total;
}


#include "main.h"
