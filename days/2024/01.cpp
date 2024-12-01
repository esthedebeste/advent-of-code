#include <algorithm>
#include <map>
#include <ranges>
#include "shared.h"


std::pair<std::array<int, 1000>, std::array<int, 1000>> parse_input(std::string_view file) {
	std::pair<std::array<int, 1000>, std::array<int, 1000>> ret;
	auto &left = ret.first;
	auto &right = ret.second;
	for (int i = 0; i < 1000; i++) {
		left[i] = parse_num(file.substr(i * 14 + 0, 5));
		right[i] = parse_num(file.substr(i * 14 + 8, 5));
	}
	std::ranges::sort(left);
	std::ranges::sort(right);
	return ret;
}

auto part_1(std::pair<std::array<int, 1000>, std::array<int, 1000>> &pair) {
	auto &[left, right] = pair;
	uint count = 0;
	for (int i = 0; i < 1000; i++) {
		count += std::abs(right[i] - left[i]);
	}
	return count;
}

auto part_2(std::pair<std::array<int, 1000>, std::array<int, 1000>> &pair) {
	auto &[left, right] = pair;
	uint count = 0;
	for (int i = 0; i < 1000; i++) {
		int value = left[i];
		for (int j = i; j < 1000; j++) {
			const auto cmp = right[j] <=> value;
			if (cmp == std::strong_ordering::greater)
				break;
			if (cmp == std::strong_ordering::equal)
				count += value;
			// if (cmp == std::strong_ordering::less)
			// 	continue;
		}
		for (int j = i - 1; j >= 0; j--) {
			const auto cmp = right[j] <=> value;
			if (cmp == std::strong_ordering::less)
				break;
			if (cmp == std::strong_ordering::equal)
				count += value;
			// if (cmp == std::strong_ordering::greater)
			// 	continue;
		}
	}
	return count;
}

int main() {
	day(parse_input, part_1, part_2);
}
