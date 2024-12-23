#include <set>
#include "shared.h"

// we use 16-bit integers to represent 2 ASCII characters at a time for pcs
constexpr uint16_t cc_to_i(const char a, const char b) { return ((a) << 7) | (b); }
constexpr std::array<char, 2> i_to_cc(const uint16_t i) { return {static_cast<char>((i >> 7)), static_cast<char>((i & 0x7F))}; }

int main() {
	using I = std::array<std::set<uint16_t>, cc_to_i('z', 'z' + 1)>;
	std::set<std::set<uint16_t>> all_groups{};
	day(
		[](std::istream &is) -> I {
			I computers{};
			do {
				char first[3], second[3];
				is >> first >> '-' >> second;
				uint16_t fi = cc_to_i(first[0], first[1]), si = cc_to_i(second[0], second[1]);
				computers[fi].insert(si);
				computers[si].insert(fi);
			} while (check(is, "\n") && std::isalpha(is.peek()));
			return computers;
		},
		[&](const I &input) {
			// find all possible groups of interconnected computers
			std::vector<std::set<uint16_t>> queue{};
			// add all possible computers to the queue at the start
			for (const auto l1 : std::string_view{"abcdefghijklmnopqrstuvwxyz"})
				for (const auto l2 : std::string_view{"abcdefghijklmnopqrstuvwxyz"})
					if (const uint16_t index = cc_to_i(l1, l2); !input[index].empty())
						queue.emplace_back(std::set{index});
			while (!queue.empty()) {
				auto set = queue.back();
				queue.pop_back();
				if (!all_groups.insert(set).second)
					continue; // already had this group
				std::set<uint16_t> to_check{};
				// find all new possible buddies
				for (const auto &other : set)
					for (const auto &member : input[other])
						if (!set.contains(member))
							to_check.insert(member);
				// remove all buddies that not everyone has
				for (const auto &other : set)
					for (auto it = to_check.begin(); it != to_check.end();)
						if (!input[other].contains(*it))
							it = to_check.erase(it);
						else
							++it;
				// add the buddies!
				for (const uint16_t index : to_check) {
					std::set<uint16_t> merge = set;
					merge.insert(index);
					queue.push_back(merge);
				}
			}

			// find the amount of groups that are 3 big and contain a t. computer
			int threes = 0;
			for (const auto &group : all_groups)
				if (group.size() == 3)
					for (const auto &computer : group)
						if (i_to_cc(computer)[0] == 't') {
							++threes;
							break;
						}
			return threes;
		},
		[&](const I &) {
			// find the largest group
			std::set<uint16_t> largest_group{};
			for (const auto &group : all_groups)
				if (group.size() > largest_group.size())
					largest_group = group;
			// print them out :)
			std::ostringstream out{};
			for (const auto &[index, computer_i] : std::views::enumerate(largest_group)) {
				if (index != 0)
					out << ',';
				auto computer_cc = i_to_cc(computer_i);
				out << computer_cc[0] << computer_cc[1];
			}
			return out.str();
		});
}
