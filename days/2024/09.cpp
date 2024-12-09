#include "shared.h"

using FILE_ID = uint16_t;
constexpr FILE_ID EMPTY = std::numeric_limits<FILE_ID>::max();
auto part_1(const std::vector<uint8_t> &nums) {
	std::vector<FILE_ID> result;
	for (size_t i = 0; i < nums.size(); ++i) {
		const bool is_empty = i & 1;
		const auto amount = nums[i];
		for (uint8_t j = 0; j < amount; ++j) result.push_back(is_empty ? EMPTY : i / 2);
	}

	auto first_empty = result.begin();
	for (auto &r : result | std::views::reverse) {
		for (auto it = first_empty; it != result.end(); ++it) {
			auto &n = *it;
			if (&n == &r)
				goto done_defragging;
			if (n == EMPTY) {
				std::swap(r, n);
				first_empty = it;
				break;
			}
		}
	}
done_defragging:

	uint64_t checksum = 0;
	for (size_t i = 0; i < result.size(); ++i) {
		const auto value = result[i];
		if (value == std::numeric_limits<FILE_ID>::max())
			break;
		checksum += static_cast<uint64_t>(value) * i;
	}
	return checksum;
}

using FILE_LEN = uint8_t;
auto part_2(const std::vector<uint8_t> &nums) {
	std::vector<std::pair<FILE_ID, FILE_LEN>> result;
	for (size_t i = 0; i < nums.size(); ++i) {
		const bool is_empty = i & 1;
		auto amount = nums[i];
		result.emplace_back(is_empty ? EMPTY : i / 2, amount);
	}

	std::array<decltype(result)::iterator, 10> first_empty;
	first_empty.fill(result.begin());
	for (auto &r : result | std::views::reverse) {
		auto &[ri, rlen] = r;
		if (ri == EMPTY)
			continue;
		for (const auto &[length_min_1, first] : std::views::enumerate(first_empty)) {
			const FILE_LEN length = static_cast<FILE_LEN>(length_min_1) + 1;
			while (first != result.end()) {
				if (first->first == EMPTY && length <= first->second)
					break;
				++first;
			}
		}
		for (auto nit = first_empty[rlen - 1]; &*nit < &r; ++nit) {
			auto &[ni, nlen] = *nit;
			if (ni != EMPTY)
				continue; // not empty
			if (nlen < rlen)
				continue; // doesn't fit
			const FILE_LEN orig_length = nlen;
			std::swap(ri, ni);
			const FILE_LEN length = nlen - rlen;
			if (length != 0) {
				nlen = rlen;
				// add an empty space of (nlen - rlen) length after nit
				if (auto next = std::next(nit); next != result.end() && next->first == EMPTY) {
					next->second += length;
				} else {
					++nit;
					result.emplace(nit, EMPTY, length);
				}
			}
			for (const auto &[length_min_1, first] : std::views::enumerate(first_empty) | std::views::take(orig_length)) {
				const FILE_LEN length = static_cast<FILE_LEN>(length_min_1) + 1;
				while (first != result.end()) {
					if (first->first == EMPTY && length <= first->second)
						break;
					++first;
				}
			}
			break;
		}
	}

	uint64_t checksum = 0;
	uint64_t i = 0;
	for (const auto &[number, amount] : result) {
		if (number != std::numeric_limits<FILE_ID>::max())
			for (auto j = 0; j < amount; ++j) checksum += number * (i + j);
		i += amount;
	}
	return checksum;
}

int main() {
	day(
		[](std::string_view str) {
			return str | std::views::transform([](auto ch) { return ch - '0'; }) | std::ranges::to<std::vector<uint8_t>>();
		},
		part_1, part_2);
}
