#include "shared.h"

struct Code {
	int numeric;
	std::string chars;
};

std::unordered_map<char, pos_t<>> positions{{'A', {0, 0}}, // alignment
																							// numbers
																						{'7', {-2, -3}},
																						{'8', {-1, -3}},
																						{'9', {0, -3}},
																						{'4', {-2, -2}},
																						{'5', {-1, -2}},
																						{'6', {0, -2}},
																						{'1', {-2, -1}},
																						{'2', {-1, -1}},
																						{'3', {0, -1}},
																						{'0', {-1, 0}},
																						// directional
																						{'^', {-1, 0}},
																						{'<', {-2, 1}},
																						{'v', {-1, 1}},
																						{'>', {0, 1}}};

std::vector<std::string> all_options(const char from = 'A', const char to = 'A') {
	static std::array<std::vector<std::string>, (1 << 16) - 1> options{};
	auto &option = options[from << 8 | to];
	if (!option.empty())
		return option;
	std::vector<std::pair<pos_t<>, std::string>> queue;
	const auto end = positions.find(to)->second;
	queue.emplace_back(positions[from], "");
	while (!queue.empty()) {
		const auto [pos, str] = queue.back();
		queue.pop_back();
		if (pos.x == -2 && pos.y == 0) {
			// evil empty spot two to the left of A
			continue;
		}
		if (pos == end) {
			option.push_back(str + 'A');
			continue;
		}
		if (pos.x > end.x)
			queue.emplace_back(pos.left(), str + '<');
		if (pos.x < end.x)
			queue.emplace_back(pos.right(), str + '>');
		if (pos.y > end.y)
			queue.emplace_back(pos.up(), str + '^');
		if (pos.y < end.y)
			queue.emplace_back(pos.down(), str + 'v');
	}
	return option;
}

uint64_t expands_to(const char from = 'A', const char to = 'A', const uint8_t robots_between = 0) {
	static std::unordered_map<uint32_t, uint64_t> cache{};
	auto &cached = cache[(from << 16) | (to << 8) | (robots_between)];
	if (cached != 0)
		return cached;
	const std::vector<std::string> &all = all_options(from, to);
	if (robots_between == 0)
		return all.front().length();
	uint64_t least_cost = std::numeric_limits<uint64_t>::max();
	for (const auto &option : all) {
		uint64_t expanded_length = expands_to('A', option[0], robots_between - 1);
		for (const auto &rng : option | std::views::slide(2)) {
			expanded_length += expands_to(rng[0], rng[1], robots_between - 1);
		}
		least_cost = min(least_cost, expanded_length);
	}
	return cached = least_cost;
}

template <uint8_t RobotsBetween> uint64_t part(const std::vector<Code> &codes) {
	uint64_t total = 0;
	for (const auto &[numeric, chars] : codes) {
		uint64_t expanded_length = expands_to('A', chars[0], RobotsBetween);
		for (const auto &rng : chars | std::views::slide(2)) {
			expanded_length += expands_to(rng[0], rng[1], RobotsBetween);
		}
		total += expanded_length * numeric;
	}
	return total;
}

int main() {
	day(lines([](std::istream &is) {
				const auto pos = is.tellg();
				Code code;
				is >> code.numeric;
				is.seekg(pos);
				is >> code.chars;
				return code;
			}),
			part<2>, part<25>);
}