#include "shared.h"
#include <execution>

struct Colors {
	int8_t red{}, green{}, blue{};
};

Colors parse_colors(std::istream &input) {
	input >> skip_until(':');
	Colors colors{};
	do {
		do {
			input.ignore(1); // space
			char valuechars[2]; // assume 1 or 2 digits for performance :tf:
			input.read(valuechars, 2); // read 2 chars
			int value;
			if (valuechars[1] != ' ') {
				// 2 digits.
				input.ignore(1); // ignore the space after the 2 digits
				// convert to int ('23' => (2 * 10) + 3 = 23)
				value = (valuechars[0] - '0') * 10 + (valuechars[1] - '0');
			} else // 1 digit
				value = valuechars[0] - '0'; // convert to int
			char color; // first char of color. r, g, or b
			input >> color;
			switch (color) {
			case 'r':
				if (value > colors.red)
					colors.red = value;
				input.ignore(2); // 'ed'
				break;
			case 'g':
				if (value > colors.green)
					colors.green = value;
				input.ignore(4); // 'reen'
				break;
			case 'b':
				if (value > colors.blue)
					colors.blue = value;
				input.ignore(3); // 'lue'
				break;
				[[unlikely]] default:
				unreachable();
			}
		} while (check(input, ","));
	} while (check(input, ";"));
	return colors;
}

int main() {
	using input = std::vector<Colors> &;
	day(
		noskipws(lines(&parse_colors)),
		[](input input) {
			int total = 0;
			for (auto [index,line] : std::ranges::views::enumerate(input))
				if (line.red <= 12 && line.green <= 13 && line.blue <= 14)
					total += index + 1;
			return total;
		},
		[](input input) {
			int total = 0;
			for (auto [index,line] : std::ranges::views::enumerate(input))
				total += line.red * line.green * line.blue;
			return total;
		});
}
