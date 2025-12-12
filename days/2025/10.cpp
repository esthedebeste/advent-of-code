#include <valarray>
#include "shared.h"

using State = uint16_t;
using Joltage = std::array<int16_t, 10>;
struct Machine {
	State target;
	std::vector<State> buttons;
	Joltage joltage;
};

using Input = std::vector<Machine>;

static Input input(std::string_view input) {
	return lines([](std::string_view machine) {
		Machine result{};
		assert_check(machine, '[');
		uint8_t i = 0;
		do {
			if (check(machine, '#'))
				result.target |= 1 << i;
			else
				assert_check(machine, '.');
			i++;
		} while (!check(machine, ']'));
		assert_check(machine, ' ');
		while (check(machine, '(')) {
			State button{};
			do {
				uint8_t x;
				auto [ptr, ec] = std::from_chars(machine.data(), machine.data() + machine.size(), x);
				assert(ec == std::errc{});
				button |= 1 << x;
				machine = std::string_view{ptr, machine.data() + machine.size()}; // advance beyond this number
			} while (check(machine, ','));
			assert_check(machine, ") ");
			result.buttons.push_back(button);
		}
		assert_check(machine, '{');
		i = 0;
		do {
			int16_t x;
			auto [ptr, ec] = std::from_chars(machine.data(), machine.data() + machine.size(), x);
			assert(ec == std::errc{});
			result.joltage[i++] = x;
			machine = std::string_view{ptr, machine.data() + machine.size()}; // advance beyond this number
		} while (check(machine, ','));
		assert_check(machine, '}');
		return result;
	})(input);
}

static int part_1(const Input &machines) {
	int total = 0;
	for (auto &machine : machines) {
		int cost = dijkstra(State{0}, machine.target, [&](State s) {
			std::vector<State> neighbours = machine.buttons;
			for (State &n : neighbours) n ^= s;
			return neighbours;
		});
		total += cost;
	}
	return total;
}

static std::string_view part_2(const Input &machines) {
	int64_t total = 0;
	for (auto &machine : machines) {
		// system of linear equations with all sorts of weird shapes
		// aargh
		total += 0;
	}
	return "N/A";
}

#include "main.h"
