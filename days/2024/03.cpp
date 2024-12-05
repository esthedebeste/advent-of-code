#include "shared.h"

using namespace std;
optional<int> read_max_three(std::string_view &file) {
	if (!isdigit(file[0]))
		return nullopt;
	int number = 0;
	int numberLength = 0;
	for (numberLength = 0; numberLength < 3 && isdigit(file[0]); numberLength++, file = file.substr(1)) number = number * 10 + file[0] - '0';
	return number;
}
int parse_mul(string_view &file) {
	if (file.size() < 8)
		return 0; // can't even be a mul(1,1) - EOF.
	file = file.substr(4); // skip mul(
	const auto number1 = read_max_three(file);
	if (!number1)
		return 0; // no number
	if (file[0] != ',')
		return 0;
	file = file.substr(1);
	const auto number2 = read_max_three(file);
	if (!number2)
		return 0; // no number
	if (file[0] != ')')
		return 0;
	file = file.substr(1);
	return *number1 * *number2;
}
int main() {
	day(
		full_file(),
		[](std::string_view file) {
			int total = 0;
			while (file.size() >= 8) {
					size_t mul_loc = file.find("mul(");
					if (mul_loc == string_view::npos)
						break;
				file = file.substr(mul_loc);
				total += parse_mul(file);
			}
			return total;
		},
		[](std::string_view file) {
			int total = 0;
			size_t dont;
			while (dont = file.find("don't()"), dont != string_view::npos) {
				std::string_view doing{file.begin(), file.begin() + dont};
				while (doing.size() >= 8) {
					size_t mul_loc = doing.find("mul(");
					if (mul_loc == string_view::npos)
						break;
					doing = doing.substr(mul_loc);
					total += parse_mul(doing);
				}
				const size_t doAgain = file.find("do()", dont);
				if (doAgain == string_view::npos)
					break;
				file = file.substr(doAgain + 4);
			}
			return total;
		});
}
