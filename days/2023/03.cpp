#include "shared.h"
#include <execution>
#include <map>

struct PosData {
	int add = 0, mul = 1;
	int amount = 0;
};

int main() {
	using input = std::tuple<std::unordered_map<pos, PosData>,
	                         std::vector<pos>,
	                         std::vector<pos>>;
	day(
		[](std::string_view string) {
			input ret;
			auto &[positions, parts, gears] = ret;
			positions.reserve(string.size());
			parts.reserve(string.size());
			gears.reserve(string.size());
			pos curr_pos{0, 0};
			auto &[x, y] = curr_pos;
			for (auto iter = string.begin(); iter != string.end(); ++iter) {
				x++;
				switch (*iter) {
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
					for (pos numberpos{numstart - 1, y - 1}; numberpos.x <= x + 1;
					     ++numberpos.x)
						for (numberpos.y = y - 1; numberpos.y <= y + 1; ++numberpos.y)
							if (numberpos.y != y || numberpos.x < numstart || numberpos.x >
							    x) {
								auto &data = positions[numberpos];
								data.add += number;
								data.amount++;
								data.mul *= number;
							}
					break;
				}
				case '\n':
					x = 0;
					y++;
					break;
				case '.':
					break;
				case '*':
					gears.push_back(curr_pos);
					[[fallthrough]];
				default:
					parts.push_back(curr_pos);
					break;
				}
			}
			return ret;
		},
		[](const input &input) {
			int total = 0;
			auto &[numbers, parts,_] = input;
			for (const auto &position : parts) {
				auto found = numbers.find(position);
				if (found != numbers.end())
					total += found->second.add;
			}
			return total;
		},
		[](const input &input) {
			uint64_t total = 0;
			auto &[numbers, _,gears] = input;
			for (const auto &position : gears) {
				auto found = numbers.find(position);
				if (found == numbers.end())
					continue;
				if (found->second.amount != 2)
					continue;
				total += found->second.mul;
			}
			return total;
		}
		);
}
