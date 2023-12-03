#include "shared.h"
#include <execution>

// My solution assigns a `Spot` value to every position in the grid.
// To parse, it loops over every position.
//  If it's a number `N`, it applies the following to the number's neighbour Spots:
//   |> Increases the `add` value by `N`
//   |> Multiplies the `mul` value by `N`
//   |> Increments the `amount` value
//  If it's a dot, it does nothing.
//  If its a part, it sets the `part` value to the character representing that part.
// To solve part 1:
//  Iterate over all Spots
//   |> Take the sum of their `add` values.
// To solve part 2:
//  Iterate over all Spots.
//   |> Filter those who have an `amount` value of two (meaning they have two number-neighbours).
//   |> Filter those who have a `part` value of `*` (meaning there is a gear on that spot).
//   |> Take the sum of their `mul` values.

struct Spot {
	// parse-time computed result of adding together all surrounding numbers
	int add = 0;
	// parse-time computed result of multiplying together all surrounding numbers
	int mul = 1;
	// amount of surrounding numbers
	int amount = 0;
	// part on this spot. '\0' means none, and '*' means a gear.
	char part = 0;
};

int main() {
	using input = std::vector<Spot>;
	day(
		[](std::string_view string) {
			const size_t width = string.find('\n');
			input positions(string.size() / (string.back() == '\n' ? width + 1 : width) * width);
			int x = 0, y = 0;
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
			}
			return total;
		});
}
