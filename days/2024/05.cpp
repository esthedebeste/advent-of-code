#include <unordered_set>
#include "shared.h"

using namespace std;

bool ordered_right(const std::unordered_set<std::pair<int, int>> &orderings, std::vector<int> &nums) {
	for (auto [i, n] : std::views::enumerate(nums)) {
		for (auto j : nums | std::views::drop(i + 1)) {
			if (!orderings.contains(std::make_pair(n, j)))
				return false;
		}
	}
	return true;
}

int main() {
	using IT = std::pair<std::unordered_set<std::pair<int, int>>, std::vector<std::vector<int>>>;
	std::vector<bool> ordered_right_cache{};
	day(
		[](std::string_view file) -> IT {
			IT ret;
			auto &[ordering, updates] = ret;
			while (file[0] != '\n') {
				ordering.emplace(parse_num(file.substr(0, 2)), parse_num(file.substr(3, 2)));
				file = file.substr(6);
			}
			file = file.substr(1);
			while (file.size() >= 5) {
				size_t line_end = file.find('\n');
				if (line_end == string::npos)
					line_end = file.size();
				std::string_view line = file.substr(0, line_end);
				file = file.substr(line_end + 1);
				if (line.size() < 2)
					break;
				std::vector<int> &update = updates.emplace_back();
				while (line.size() > 2) {
					update.emplace_back(parse_num(line.substr(0, 2)));
					line = line.substr(3);
				}
				update.emplace_back(parse_num(line.substr(0, 2)));
			}
			return ret;
		},
		[&](IT &input) {
			auto &[ordering, updates] = input;
			int total = 0;
			for (auto &update : updates)
				if (ordered_right(ordering, update))
					total += update[update.size() / 2];
			return total;
		},
		[&](IT &input) {
			auto &[ordering, updates] = input;
			int total = 0;
			for (auto &update : updates) {
				if (ordered_right(ordering, update))
					continue;
				std::ranges::sort(update, [&](auto &a, auto &b) {
					return ordering.contains(std::make_pair(a, b));
				});
				total += update[update.size() / 2];
			}
			return total;
		});
}
