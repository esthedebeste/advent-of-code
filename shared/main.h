#include "shared.h"
really_always_inline void part_1() {}
really_always_inline void part_2() {}
template <class I>
	requires requires(const I &i) { ::part_1(i); }
really_always_inline void call_part_1(I &input, std::chrono::high_resolution_clock::duration &total_time) {
	std::cout << "Running part 1..." << std::endl;
	const auto start = std::chrono::high_resolution_clock::now();
	auto res = part_1(input);
	const auto end = std::chrono::high_resolution_clock::now();
	total_time += end - start;
	std::cout << "Done with part 1! Result: `" << res << "` (" << std::chrono::duration_cast<dmilli>(end - start).count() << "ms)"
						<< std::endl;
}
template <class I> really_always_inline void call_part_1(I &, std::chrono::high_resolution_clock::duration &) {}
template <class I>
	requires requires(const I &i) { ::part_2(i); }
really_always_inline void call_part_2(I &input, std::chrono::high_resolution_clock::duration &total_time) {
	std::cout << "Running part 2..." << std::endl;
	const auto start = std::chrono::high_resolution_clock::now();
	auto res = part_2(input);
	const auto end = std::chrono::high_resolution_clock::now();
	total_time += end - start;
	std::cout << "Done with part 2! Result: `" << res << "` (" << std::chrono::duration_cast<dmilli>(end - start).count() << "ms)"
						<< std::endl;
}
template <class I> really_always_inline void call_part_2(I &, std::chrono::high_resolution_clock::duration &) {}

int main() {
	std::ios::sync_with_stdio(false);
	std::cout << "Running day " AOC_DAY_STR "...\nProcessing input..."
						<< std::endl;
	// we don't use cin
	std::cin.tie(nullptr);
	std::cout.tie(nullptr);
	std::clog.tie(nullptr);

	std::chrono::high_resolution_clock::duration total_time{};
	const auto start = std::chrono::high_resolution_clock::now();
	auto input = internal::parse_input(::input);
	const auto end = std::chrono::high_resolution_clock::now();
	total_time += end - start;
	std::cout << "Done processing input! (" << std::chrono::duration_cast<dmilli>(end - start).count() << "ms)" << std::endl;
	call_part_1(input, total_time);
	call_part_2(input, total_time);
	std::cout << "Done running day " AOC_DAY_STR " in " << std::chrono::duration_cast<dmilli>(total_time).count() << "ms!\n";
	return 0;
}
