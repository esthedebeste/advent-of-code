#include "shared.h"

enum Operation : char { Add = '+', Multiply = '*' };

static std::vector<std::string_view> input(std::string_view input) {
	// there is no way im parallel parsing both interpretations. just parse seperately in both parts
	return lines([](std::string_view s) { return s; })(input);
}

static uint64_t part_1(std::vector<std::string_view> input) {
	std::string_view &operations = input.back();
	uint64_t sum = 0;
	do {
		Operation operation = static_cast<Operation>(operations.front());
		operations = operations.substr(1);
		size_t width = 0;
		while (check(operations, ' ')) width++;
		// trimmed EOF means no padding spaces at the end. can still be derived from the rest of the input though :)
		if (operations.empty())
			for (std::string_view &n : input) width = std::max(width, n.size());
		uint64_t result{};
		for (size_t i = 0; i < input.size() - 1; i++) {
			std::string_view &numbers = input[i];
			uint64_t number;
			size_t this_width = width;
			while (check(numbers, ' ')) this_width--;
			std::from_chars(numbers.data(), numbers.data() + this_width, number, 10);
			numbers = numbers.substr(std::min(this_width + 1, numbers.size()));
			if (i == 0) {
				result = number;
				continue;
			}
			switch (operation) {
			case Add:
				result += number;
				break;
			case Multiply:
				result *= number;
				break;
			default:
				unreachable();
			}
		}
		sum += result;
	} while (!operations.empty());
	return sum;
}

static uint64_t part_2(std::vector<std::string_view> input) {
	std::string_view &operations = input.back();
	uint64_t sum = 0;
	do {
		Operation operation = static_cast<Operation>(operations.front());
		operations = operations.substr(1);
		size_t width = 0;
		while (check(operations, ' ')) width++;
		// trimmed EOF means no padding spaces at the end. can still be derived from the rest of the input though :)
		if (operations.empty())
			for (std::string_view &n : input) width = std::max(width, n.size());
		uint64_t result{};
		for (size_t x = 0; x < width; x++) {
			uint64_t number = 0;
			for (size_t i = 0; i < input.size() - 1; i++) {
				std::string_view &numbers = input[i];
				char c = numbers[x];
				if (!std::isdigit(c))
					continue;
				number = number * 10 + c - '0';
			}
			if (x == 0) {
				result = number;
				continue;
			}
			switch (operation) {
			case Add:
				result += number;
				break;
			case Multiply:
				result *= number;
				break;
			default:
				unreachable();
			}
		}
		for (size_t i = 0; i < input.size() - 1; i++) {
			std::string_view &numbers = input[i];
			numbers = numbers.substr(std::min(width + 1, numbers.size()));
		}
		sum += result;
	} while (!operations.empty());
	return sum;
}

#include "main.h"
