#include "shared.h"

using K = std::array<uint8_t, 5>;
using KV = std::vector<K>;
using KVP = std::pair<KV, KV>;
KVP input(std::string_view str) {
	KVP result{};
	auto &[keys, locks] = result;
	keys.reserve(250), locks.reserve(250);
	do {
		auto &key = str[0] & 1 ? keys.emplace_back() : locks.emplace_back();
		for (int i = 0; i < 5; i++)
			for (auto j = 0; j < 5; j++) key[j] += str[6 + 6 * i + j] & 1;
		str = str.substr(42);
	} while (check(str, '\n'));
	return result;
}

int part_1(const KVP &kvp) {
	auto &[keys, locks] = kvp;
	int fit = 0;
	for (auto &key : keys)
		for (auto &lock : locks)
			if (key[0] + lock[0] <= 5 && key[1] + lock[1] <= 5 && key[2] + lock[2] <= 5 && key[3] + lock[3] <= 5 && key[4] + lock[4] <= 5)
				++fit;
	return fit;
}

#include "main.h"