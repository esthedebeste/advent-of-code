#include "shared.h"

using namespace std;
bool is_bad(const bool direction, const int diff) {
	if (signbit(diff) != direction)
		return true;
	if (diff == 0 || std::abs(diff) > 3)
		return true;
	return false;
}

bool is_good(const vector<int> &row) {
	const bool direction = signbit(row[1] - row[0]);
	return ranges::none_of(views::adjacent<2>(row), [=](auto x) {
		auto [last, now] = x;
		return is_bad(direction, now - last);
	});
}
bool is_good_joker(vector<int> &row) {
	if (is_good(row))
		return true;
	for (int i = 0; i < row.size(); i++) {
		vector<int> without{};
		without.reserve(row.size() - 1);
		without.insert(without.end(), row.begin(), row.begin() + i);
		without.insert(without.end(), row.begin() + i + 1, row.end());
		if (is_good(without))
			return true;
	}
	return false;
}
int main() {
	day(
		lines(split_by(' ', parse<int>)), [&](vector<vector<int>> &input) { return ranges::count(input, true, is_good); },
		[&](vector<vector<int>> &input) { return ranges::count(input, true, is_good_joker); });
}
