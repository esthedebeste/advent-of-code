#include <map>
#include "shared.h"

using stone_t = uint64_t;
stone_t number_length(stone_t numb) {
	stone_t length = 0;
	do {
		numb /= 10;
		length++;
	} while (numb != 0);
	return length;
}

std::map<std::pair<stone_t, uint8_t>, stone_t> cache{};
stone_t how_many_stones_is(const stone_t stone, const uint8_t iter_c) {
	if (const auto found = cache.find({stone, iter_c}); found != cache.end())
		return found->second;
	if (iter_c == 0)
		return 1;
	if (stone == 0) {
		return cache[{stone, iter_c}] = how_many_stones_is(1, iter_c - 1);
	}
	if (auto length = number_length(stone); (length & 1) == 0) {
		const stone_t bigness = std::pow(10, length / 2);
		const stone_t left_side = stone / bigness;
		const stone_t right_side = stone % bigness;
		return cache[{stone, iter_c}] = how_many_stones_is(left_side, iter_c - 1) + how_many_stones_is(right_side, iter_c - 1);
	}
	return cache[{stone, iter_c}] = how_many_stones_is(stone * 2024, iter_c - 1);
}

int main() {
	day(
		[](std::istream &stream) -> std::vector<stone_t> {
			std::vector<stone_t> numbers{};
			while (stream) {
				stone_t number = std::numeric_limits<stone_t>::max();
				stream >> number;
				if (number == std::numeric_limits<stone_t>::max())
					break; // read fail :(
				numbers.push_back(number);
			}
			return numbers;
		},
		[](const std::vector<stone_t>& stones) {
			return sum<stone_t>(stones, [](const stone_t stone) { return how_many_stones_is(stone, 25); });
	},
	[](const std::vector<stone_t>& stones) {
			return sum<stone_t>(stones, [](const stone_t stone) { return how_many_stones_is(stone, 75); });
	});
}
