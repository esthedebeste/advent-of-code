#ifndef NDEBUG
#define SHOULD_PRINT_FIELD
#endif
#define default_pos_t int
#include "shared.h"

using dpt = pos_t<>::member_type;

struct Map {
	std::string field;
	dpt width;
	dpt height;
	pos_t<> robot;
	std::vector<pos_t<>> moves;
	char &at(const pos_t<> &pos) { return field[pos.to_1d(width)]; }
	char &operator[](const pos_t<> &pos) { return at(pos); }
	const char &at(const pos_t<> &pos) const { return field[pos.to_1d(width)]; }
	const char &operator[](const pos_t<> &pos) const { return at(pos); }

	void do_move(const pos_t<> &move) {
		std::vector queue{robot};
		std::unordered_map<pos_t<>, char> to_move{{robot, '.'}};
		while (!queue.empty()) {
			const auto pos = queue.back();
			queue.pop_back();
			const auto moved = pos + move;
			if (to_move.contains(moved))
				continue;
			char &c = at(moved);
			if (c == '.') {
			} else if (c == 'O') {
				to_move.emplace(moved, c);
				queue.emplace_back(moved);
			} else if (c == '[') {
				to_move.emplace(moved, c);
				to_move.emplace(moved.right(), at(moved.right()));
				queue.emplace_back(moved);
				queue.emplace_back(moved.right());
			} else if (c == ']') {
				to_move.emplace(moved, c);
				to_move.emplace(moved.left(), at(moved.left()));
				queue.emplace_back(moved);
				queue.emplace_back(moved.left());
			} else if (c == '#') {
				return; // dont move
			}
		}
		for (const auto &loc : to_move | std::views::keys) at(loc) = '.';
		for (const auto &[loc, c] : to_move) at(loc + move) = c;
		robot += move;
	}

	void print() const {
#ifndef SHOULD_PRINT_FIELD
		return;
#endif
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++)
				if (const pos_t pos{x, y}; pos == robot)
					std::cout << '@';
				else
					std::cout << (*this)[pos];
			std::cout << '\n';
		}
	}

	[[nodiscard]]
	int count_gps() const {
		int total = 0;
		for (const auto &[i, c] : std::views::enumerate(field)) {
			if (c != 'O' && c != '[')
				continue;
			auto [x, y] = pos_t<>::from_1d(width, i);
			total += 100 * y + x;
		}
		return total;
	}

	int solve() {
		for (const auto &move : moves) do_move(move);
		print();
		return count_gps();
	}
};

Map resize(const Map &map) {
	Map resized{.field = std::string(map.width * 2 * map.height, '.'),
							.width = map.width * 2,
							.height = map.height,
							.robot = {map.robot.x * 2, map.robot.y},
							.moves = map.moves};
	for (const auto &[i, c] : std::views::enumerate(map.field)) {
		if (c != 'O')
			resized.field[i * 2] = resized.field[i * 2 + 1] = c;
		else {
			resized.field[i * 2] = '[';
			resized.field[i * 2 + 1] = ']';
		}
	}
	return resized;
}

int main() {
	day(
		[](std::string_view inview) {
			std::string input{inview};
			Map map{};
			map.height = map.width = input.find('\n');
			std::erase(input, '\n');
			const size_t robot_i = input.find('@');
			input[robot_i] = '.';
			map.robot = pos_t<>::from_1d(map.width, robot_i);
			size_t end_i = map.width * map.height;
			for (const char &c : input | std::views::drop(end_i)) switch (c) {
				case '^':
					map.moves.emplace_back(0, -1);
					break;
				case 'v':
					map.moves.emplace_back(0, 1);
					break;
				case '<':
					map.moves.emplace_back(-1, 0);
					break;
				case '>':
					map.moves.emplace_back(1, 0);
					break;
				default:
					unreachable();
				}
			input.resize(end_i);
			map.field = input;
			return map;
		},
		[](Map map) { return map.solve(); }, [](const Map &map) { return resize(map).solve(); });
}
