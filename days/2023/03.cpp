#include "shared.h"
#include <execution>

struct Number {
	int add = 0, mul = 1;
	int amount = 0;
	char part = 0; // 0 = none, '*' = gear, etc.
};

int main() {
	using input = std::vector<Number>;
	day(
		[](std::string_view string) {
			const size_t width = string.find('\n');
			input positions(string.size() / (string.back() == '\n' ? width + 1 : width) * width);
			pos curr_pos{ 0, 0 };
			auto &[x, y] = curr_pos;
			for (auto iter = string.begin(); iter != string.end(); ++iter) {
				x++;
				switch (const char c = *iter) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': {
					int number = 0;
					int numwidth = 0;
					do {
						number = number * 10 + *iter - '0';
						numwidth++;
						++iter;
					} while (isdigit(*iter));
					--iter;
					const int numstart = x;
					x += numwidth - 1;
					for (int ypos = max(0, y - 1); ypos <= y + 1; ++ypos)
						for (int xpos = min(max(0, numstart - 1), width), xmax = min(x + 1, width); xpos <= xmax; ++xpos)
							if (ypos != y || xpos < numstart || xpos > x) {
								auto &[add, mul, amount, _part] = positions[ypos * width + xpos];
								add += number, mul *= number, ++amount;
							}
					break;
				}
				case '\n':
					x = 0;
					y++;
					break;
				case '.':
					break;
				default:
					positions[y * width + x].part = c;
					break;
				}
			}
			return positions;
		},
		[](const input &input) {
			int total = 0;
			for (const auto &[add, mul, amount, part] : input) {
				if (part) total += add;
			}
			return total;
		},
		[](const input &input) {
			uint64_t total = 0;
			for (const auto &[add, mul, amount, part] : input) {
				if (part == '*' && amount == 2) total += mul;
				return total;
			}
		});
}
