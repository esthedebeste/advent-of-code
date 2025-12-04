#include "shared.h"

std::vector<std::vector<int8_t>> input(std::string_view input) {
	std::vector<std::vector<int8_t>> result{};
	while (input.size() > 1) {
		size_t newline = input.find('\n', 2);
		if (newline == std::string_view::npos)
			newline = input.size();
		std::string_view before_newline = input.substr(0, newline);
		std::vector<int8_t> &v = result.emplace_back();
		for (char c : before_newline) v.push_back(c - '0');
		if (newline == input.size())
			break;
		input = input.substr(newline + 1);
	}
	return result;
}

int part_1(const std::vector<std::vector<int8_t>>& input) { 
	int total = 0;
	for (auto &line : input) {
		auto first_digit_it = std::max_element(line.begin(), line.end() - 1);
		auto second_digit_it = std::max_element(first_digit_it + 1, line.end());
		total += *first_digit_it * 10 + *second_digit_it;
	}
	return total;
}

int64_t part_2(const std::vector<std::vector<int8_t>> &input) {
	int64_t total = 0;
	for (auto &line : input) {
		auto digit_it = line.begin();
		int64_t this_line_solution = 0;
		for (int i = 0; i < 12; i++) {
			int need_to_keep_around = 11 - i;
			digit_it = std::max_element(digit_it, line.end() - need_to_keep_around);
			this_line_solution = this_line_solution * 10 + *digit_it;
			++digit_it;
		}
		total += this_line_solution;
	}
	return total;
}

#include "main.h"
