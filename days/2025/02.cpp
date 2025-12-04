#include <atomic>
#include <cmath>
#include <thread_pool/thread_pool.h>
#include "shared.h"


bool is_double(uint64_t x) {
	int length = intwidth(x);
	if (length & 1) // odd length cant repeat
		return false;

	int sequence_length = length / 2;
	uint64_t tenpow = 1;
	for (int x = 0; x < sequence_length; x++) tenpow *= 10;
	uint64_t high_part = x / tenpow; // xxx---
	uint64_t low_part = x % tenpow; // ---xxx
	return high_part == low_part; // if xxx--- = ----
}

bool is_double_or_more(uint64_t x) {
	int length = intwidth(x);
	if (is_double(x))
		return true;
	for (int i = 3; i <= length; i++) {
		if (length % i != 0) // if number isnt divisible into i groups, continue
			continue;
		int sequence_length = length / i;
		uint64_t tenpow = 10; // 10^sequence_length
		for (int x = 1; x < sequence_length; x++) tenpow *= 10;
		uint64_t high_part = x / tenpow; // xxxx--
		uint64_t low_part = x % tenpow; // ----xx
		do {
			uint64_t div = high_part / tenpow; // xx----
			uint64_t mod = high_part % tenpow; // --xx--
			if (mod != low_part) // if --xx-- != ----xx then we do not have a repeater
				goto next_partitioning;
			high_part = div; // and continue
		} while (high_part != 0); // xx / 100 = 0
		return true; // it isnt not a repeater, so it is a repeater
	next_partitioning:;
	}
	return false; // tried all group sizes, didnt find anything
}

int main() {
	day(
		[](std::string_view input) {
			std::vector<std::pair<uint64_t, uint64_t>> result;
			while (input.size() >= 3) {
				std::size_t dash = input.find('-', 1), comma = input.find(',', 3);
				if (comma == std::string_view::npos) { // no commas left, go until end of string
					std::string_view first = input.substr(0, dash);
					std::string_view second = input.substr(dash + 1);
					result.emplace_back(parse_num<uint64_t>(first), parse_num<uint64_t>(second));
					break;
				}

				std::string_view first = input.substr(0, dash);
				std::string_view second = input.substr(dash + 1, comma - dash - 1);
				result.emplace_back(parse_num<uint64_t>(first), parse_num<uint64_t>(second));
				input = input.substr(comma + 1);
			}
			return result;
		},
		[](const std::vector<std::pair<uint64_t, uint64_t>> &s) {
			std::atomic_uint64_t invalid = 0;
			dp::thread_pool pool{};
			for (auto [start, end] : s) {
				pool.enqueue_detach([=, &invalid] {
					uint64_t total = 0;
					for (uint64_t x = start; x <= end; x++)
						if (is_double(x))
							total += x;
					invalid.fetch_add(total);
				});
			}
			pool.wait_for_tasks();
			return invalid.load();
		},
		[](const std::vector<std::pair<uint64_t, uint64_t>> &s) {
			std::atomic_uint64_t invalid = 0;
			dp::thread_pool pool{};
			for (auto [start, end] : s) {
				pool.enqueue_detach([=, &invalid] {
					uint64_t total = 0;
					for (uint64_t x = start; x <= end; x++)
						if (is_double_or_more(x))
							total += x;
					invalid.fetch_add(total);
				});
			}
			pool.wait_for_tasks();
			return invalid.load();
		});
}
