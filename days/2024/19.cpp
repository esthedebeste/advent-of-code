#include <future>
#include <thread_pool/thread_pool.h>
#include "shared.h"

struct Input {
	std::vector<std::string> towels;
	std::vector<std::string> designs;
};

using poss_t = uint64_t;
// thread_local, testing it with a shared_mutex for a global cache ended up being slower :P
thread_local std::unordered_map<std::string_view, poss_t> possibilities_cache{};
poss_t possibilities(const std::string_view design, const std::vector<std::string> &towels) {
	if (design.empty())
		return 1;
	if (const auto it = possibilities_cache.find(design); it != possibilities_cache.end())
		return it->second;
	poss_t total = 0;
	for (auto &towel : towels)
		if (design.starts_with(towel))
			total += possibilities({design.begin() + towel.size(), design.end()}, towels);
	return possibilities_cache[design] = total;
}

int main() {
	std::vector<poss_t> possible{};
	day(
		[](std::istream &is) {
			Input input;
			do {
				auto &towel = input.towels.emplace_back();
				while (isalpha(is.peek())) towel += is.get();
			} while (check(is, ", "));
			is >> "\n";
			while (check(is, '\n') && std::isalpha(is.peek())) {
				is >> input.designs.emplace_back();
			}
			return input;
		},
		[&](const Input &input) {
			dp::thread_pool pool{};
			std::vector<std::future<poss_t>> futures;
			futures.reserve(input.designs.size());
			for (std::string_view design : input.designs)
				futures.push_back(pool.enqueue([&, design] { return possibilities(design, input.towels); }));
			possible.reserve(futures.size());
			for (auto &f : futures) possible.push_back(f.get());
			return sum<poss_t>(possible, [](const poss_t n) { return n > 0; });
		},
		[&](const Input &) { return sum<poss_t>(possible); });
}
