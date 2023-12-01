#include "shared.h"
#include <array>
#include <execution>

struct Input {
	int naive, words;
};

std::array<std::string_view, 9> numbers{"one", "two", "three", "four", "five",
                                        "six", "seven", "eight", "nine"};

int main() {
	using input = std::vector<Input> &;
	day(
		lines([](const std::string &input) {
			int startNaive = -1, startWords = -1;
			for (std::string_view view = input; !view.empty();
			     view = view.substr(1)) {
				if (std::isdigit(view.front())) {
					startNaive = view.front() - '0';
					if (startWords == -1)
						startWords = startNaive;
					break;
				}
				if (startWords == -1)
					for (int i = 0; i < numbers.size(); ++i) {
						if (view.starts_with(numbers[i])) {
							startWords = i + 1;
						}
					}
			}
			int endNaive = -1, endWords = -1;
			for (std::string_view view = input; !view.empty();
			     view = view.substr(0, view.size() - 1)) {
				if (std::isdigit(view.back())) {
					endNaive = view.back() - '0';
					if (endWords == -1)
						endWords = endNaive;
					break;
				}
				if (endWords == -1)
					for (int i = 0; i < numbers.size(); ++i) {
						if (view.ends_with(numbers[i])) {
							endWords = i + 1;
						}
					}
			}
			return Input{startNaive * 10 + endNaive, startWords * 10 + endWords};
		}),
		[](input input) {
			return std::reduce(input.begin(), input.end(), 0,
			                   [](int a, auto b) { return a + b.naive; });
		},
		[](input input) {
			return std::reduce(input.begin(), input.end(), 0,
			                   [](int a, auto b) { return a + b.words; });
		});
}
