#include "shared.h"
#include <execution>

struct Card {
	int8_t matching = 0;
	// for part 2 so that we dont need to create another vector just for amounts
	int amount = 1;
};


static int8_t parse_card(const char *&input, const char *end, std::vector<uint16_t> &winning) {
	while (input[0] != ':') ++input;
	input += 2;
	winning.clear();
	while (input[0] <= 64) {
		winning.push_back(reinterpret_cast<const uint16_t &>(input[0]));
		input += 3;
	}
	input += 2;
	int8_t matching = 0;
	while (input[0] <= 64 && input < end) {
		if (const uint16_t card = reinterpret_cast<const uint16_t &>(input[0]);
			std::find(winning.begin(), winning.end(), card) != winning.end()) {
			++matching;
		}
		input += 3;
	}
	return matching;
}

int main() {
	using input = std::vector<Card>;
	day(
		[](const std::string_view file) {
			input cards;
			std::vector<uint16_t> little_reused_temp_vector;
			const char *start = &file.front(), *end = &file.back();
			do cards.emplace_back(parse_card(start, end, little_reused_temp_vector), 1);
			while (start < end);
			return cards;
		},
		[](const input &cards) {
			int total = 0;
			for (const auto [matching, _] : cards) if (matching > 0) total += 1 << (matching - 1);
			return total;
		},
		[](input &cards) {
			for (const auto [index, card] : std::views::enumerate(cards)) {
				for (size_t i = index + 1, maxi = min(index + 1 + card.matching, cards.size()); i < maxi; ++i) {
					cards[i].amount += card.amount;
				}
			}
			return std::reduce(cards.begin(), cards.end(), 0,
												 [](int a, auto b) { return a + b.amount; });
		});
}
