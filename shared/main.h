#pragma once
#include "shared.h"
really_always_inline void part_1() {}
really_always_inline void part_2() {}
template <class I>
	requires requires(const I &i) { ::part_1(i); }
really_always_inline void call_part_1(I &input, std::chrono::high_resolution_clock::duration &total_time) {
	auto res = internal::timeit([&] { return part_1(input); }, "Running part 1...", "Done with part 1!", total_time);
	std::cout << "Part 1 result: " << res << std::endl;
}
template <class I> really_always_inline void call_part_1(I &, std::chrono::high_resolution_clock::duration &) {}
template <class I>
	requires requires(const I &i) { ::part_2(i); }
really_always_inline void call_part_2(I &input, std::chrono::high_resolution_clock::duration &total_time) {
	auto res = internal::timeit([&] { return part_2(input); }, "Running part 2...", "Done with part 2!", total_time);
	std::cout << "Part 2 result: " << res << std::endl;
}
template <class I> really_always_inline void call_part_2(I &, std::chrono::high_resolution_clock::duration &) {}

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
int main() {
	std::ios::sync_with_stdio(false);
	std::cout << "Running day " AOC_DAY_STR "..." << std::endl;
	// we don't use cin
	std::cin.tie(nullptr);
	std::cout.tie(nullptr);
	std::clog.tie(nullptr);

	std::chrono::high_resolution_clock::duration total_time{};
	auto input =
		internal::timeit([&] { return internal::parse_input(::input); }, "Processing input...", "Done processing input!", total_time);
	call_part_1(input, total_time);
	call_part_2(input, total_time);
	std::cout << "Done running day " AOC_DAY_STR " in " << std::chrono::duration_cast<dmilli>(total_time).count() << "ms!\n";
	return 0;
}
