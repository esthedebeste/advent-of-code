#include <bitset>
#include <thread_pool/thread_pool.h>
#include "shared.h"

constexpr int prune(const int curr) { return curr & ((1 << 24) - 1); }
constexpr int next_secret(int curr) {
	curr = prune(curr ^ (curr << 6));
	curr = prune(curr ^ (curr >> 5));
	curr = prune(curr ^ (curr << 11));
	return curr;
}

int main() {
	dp::thread_pool pool{};
	day(
		lines(parse<int>),
		[&](const std::vector<int> &lines) {
			std::atomic_uint64_t curr{0};
			for (int start : lines)
				pool.enqueue_detach([&, start] {
					int n = start;
					for (int i = 0; i < 2000; i++) n = next_secret(n);
					curr += n;
				});
			pool.wait_for_tasks();
			return curr.load();
		},
		[&](const std::vector<int> &lines) {
			std::array<std::atomic_int, (1 << 16) - 1> sums{};
			for (const int start : lines) {
				pool.enqueue_detach([&, start] {
					std::bitset<(1 << 16) - 1> this_line{};
					this_line.reset();
					int last = start;
					uint8_t last_price = std::numeric_limits<uint8_t>::max();
					uint16_t changes = std::numeric_limits<uint16_t>::max();
					for (int i = 0; i < 2000; i++) {
						const auto next = next_secret(last);
						const uint8_t price = next % 10;
						changes <<= 4;
						changes |= static_cast<uint8_t>(price - last_price) & 0xF;
						last = next;
						last_price = price;
						if (i > 4 && !this_line[changes]) {
							this_line[changes] = true;
							sums[changes] += price;
						}
					}
				});
			}
			pool.wait_for_tasks();
			int max = 0;
			for (const auto &sum : sums)
				if (const auto value = sum.load(); value > max)
					max = value;
			return max;
		});
}
