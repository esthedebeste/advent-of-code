#include "shared.h"

constexpr char XMAS[5]{"XMAS"};
template <class T> size_t count_occurrences(T view) {
	size_t count = 0;
	uint_fast8_t xmas_progress = 0;
	for (const char ch : view)
		if (ch == XMAS[xmas_progress]) {
			if (++xmas_progress == 4) {
				count++;
				xmas_progress = 0;
			}
		} else
			xmas_progress = ch == 'X';
	return count;
}
int main() {
	day(
		[](std::string_view input) {
			std::pair<std::ptrdiff_t, std::string> result{0, input};
			auto &[size, output] = result;
			size = input.find('\n');
			std::erase(output, '\n');
			return std::make_pair(size, output);
		},
		[](std::pair<std::ptrdiff_t, std::string> &input) {
			auto &[size, text] = input;
			size_t occurrences = 0;
			for (int y = 0; y < size; ++y) {
				const std::string_view line{text.begin() + y * size, text.begin() + y * size + size};
				occurrences += count_occurrences(line);
				occurrences += count_occurrences(std::views::reverse(line));
			}
			for (int x = 0; x < size; ++x) {
				std::string vertical{};
				for (int y = 0; y < size; ++y) vertical += text[y * size + x];
				occurrences += count_occurrences(vertical);
				occurrences += count_occurrences(std::views::reverse(vertical));
			}
			for (auto d = 1 - size; d < size; ++d) {
				std::string diagonal{};
				for (int y = 0; y < size; ++y)
					if (const int x = d + y; x >= 0 && x < size)
						diagonal += text[y * size + x];
				occurrences += count_occurrences(diagonal);
				occurrences += count_occurrences(std::views::reverse(diagonal));
			}
			for (std::ptrdiff_t d = 0; d <= (size - 1) * 2; ++d) {
				std::string diagonal{};
				for (std::ptrdiff_t y = 0; y < size; ++y)
					if (const auto x = d - y; x >= 0 && x < size)
						diagonal += text[y * size + x];
				occurrences += count_occurrences(diagonal);
				occurrences += count_occurrences(std::views::reverse(diagonal));
			}
			return occurrences;
		},
		[](std::pair<std::ptrdiff_t, std::string> &input) {
			auto &[size, text] = input;
			size_t occurrences = 0;
			for (auto y = 1; y < size - 1; ++y) {
				for (auto x = 1; x < size - 1; ++x) {
					if (text[y * size + x] != 'A')
						continue;
					const char tl = text[(y - 1) * size + (x - 1)];
					const char tr = text[(y - 1) * size + (x + 1)];
					const char bl = text[(y + 1) * size + (x - 1)];
					const char br = text[(y + 1) * size + (x + 1)];
					if (!(tr == 'M' && bl == 'S' || tr == 'S' && bl == 'M'))
						continue;
					if (!(tl == 'M' && br == 'S' || tl == 'S' && br == 'M'))
						continue;
					occurrences++;
				}
			}
			return occurrences;
		});
}
