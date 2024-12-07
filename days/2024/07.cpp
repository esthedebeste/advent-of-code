#include "shared.h"

uint64_t concat_numbers(uint64_t a, uint64_t b) {
	uint64_t bigness = 1;
	uint64_t b_copy = b;
	do {
		b_copy /= 10;
		bigness *= 10;
	} while (b_copy != 0);
	return a * bigness + b;
}

uint64_t add(const uint64_t a, const uint64_t b) {
	return a + b;
}

uint64_t mul(const uint64_t a, const uint64_t b) {
	return a * b;
}

using P = std::pair<uint64_t, std::vector<uint64_t>>;
using IT = std::vector<P>;

std::vector<std::pair<uint64_t, size_t>> queue{};
uint64_t solve(const IT &s, auto... operation) {
	uint64_t total = 0;
	for (const P &p : s) {
		queue.clear();
		const auto &[test, values] = p;
		queue.emplace_back(values[0], 1);
		while (!queue.empty()) {
			auto [number, nextI] = queue.back();
			queue.pop_back();
			if (number > test)
				continue;
			if (nextI == values.size()) {
				if (number == test) [[unlikely]] {
					total += test;
					goto next_p;
				}
				continue;
			}
			const auto next = values[nextI];
			nextI++;
			(queue.emplace_back(operation(number, next), nextI), ...);
		}
	next_p:
	}
	return total;
}

using namespace std;
int main() {
	day(
		lines([](std::istream &is) -> P {
			P out{};
			is >> out.first;
			is >> ":";
			while (check(is, " ")) is >> out.second.emplace_back();
			return out;
		}),
		[](const IT &s) { return solve(s, add, mul); },
		[](const IT &s) { return solve(s, concat_numbers, add, mul); });
}
