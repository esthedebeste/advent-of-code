#define default_pos_t std::ptrdiff_t
#include <unordered_map>
#include <unordered_set>
#include "shared.h"

using namespace std;

std::unordered_map<std::pair<pos_t<>, pos_t<>>, size_t> visited{};
bool loops(default_pos_t size, const std::string &field, pos_t<> guard) {
	visited.clear();
	pos_t<> direction{0, -1}; // upward at the start
	while (true) {
		auto emplaced =	visited.emplace(std::make_pair(guard, direction), static_cast<size_t>(0));
		if (!emplaced.second)
			return true; // not actually added, guard has already been here, loop.
		auto& length = emplaced.first->second;
		pos_t<> next = guard + direction;
		do {
			if (next.x < 0 || next.x >= size || next.y < 0 || next.y >= size)
				return false;
			if (field[next.to_1d(size)] == '#') {
				direction = direction.rotate_right_90();
				break;
			}
			guard = next;
			next = guard + direction;
			length++;
		} while (true);
	}
}

int main() {
	using IT = std::tuple<default_pos_t, std::string, pos_t<>>;
	std::unordered_set<default_pos_t> setted{};
	day(
		[](std::string_view input) -> IT {
			IT result{0, input, 0};
			auto &[size, output, guard] = result;
			size = static_cast<default_pos_t>(input.find('\n'));
			std::erase(output, '\n');
			auto guardi = static_cast<default_pos_t>(output.find('^')); // find the guard
			guard = {guardi % size, guardi / size};
			return result;
		},
		[&](IT &in) -> size_t {
			auto &[size, field, guard] = in;
			if (loops(size, field, guard))
				return 0;
			for (const auto &[posdir, length] : visited) {
				const auto &[pos, dir] = posdir;
				for (auto way = pos; way != pos + dir * (length + 1); way += dir)
					setted.emplace(way.to_1d(size));
			}
			return setted.size();
		},
		[&](IT &in) {
			auto &[size, field, guard] = in;
			setted.erase(guard.to_1d(size));
			size_t loop_locs = 0;
			for (const auto &index : setted) {
				field[index] = '#';
				if (loops(size, field, guard))
					loop_locs++;
				field[index] = '.';
			}
			return loop_locs;
		});
}
