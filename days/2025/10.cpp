#include <glpk.h>
#include <valarray>
#include "shared.h"

using State = uint16_t;
using Joltage = std::vector<int16_t>;

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
		do {
			int16_t x;
			auto [ptr, ec] = std::from_chars(machine.data(), machine.data() + machine.size(), x);
			assert(ec == std::errc{});
			result.joltage.push_back(x);
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

static int64_t part_2(const Input &machines) {
	int64_t total = 0;
	for (auto &machine : machines) {
		const int rows = machine.joltage.size();
		const int columns = machine.buttons.size();
		glp_prob *problem = glp_create_prob();
		glp_set_obj_dir(problem, GLP_MIN);
		glp_add_rows(problem, rows);
		for (int row = 0; row < rows; ++row) {
			glp_set_row_bnds(problem, row + 1, GLP_FX, machine.joltage[row], machine.joltage[row]);
		}
		glp_add_cols(problem, columns);
		for (int column = 0; column < columns; ++column) {
			glp_set_col_bnds(problem, column + 1, GLP_LO, 0, 0);
			glp_set_col_kind(problem, column + 1, GLP_IV);
			glp_set_obj_coef(problem, column + 1, 1);
		}
		std::vector<int> ia{}, ja{};
		std::vector<double> ar{};
		ia.reserve(rows * columns), ja.reserve(rows * columns), ar.reserve(rows * columns);
		for (int row = 0; row < rows; ++row) {
			for (int column = 0; column < columns; ++column) {
				ia.push_back(row + 1);
				ja.push_back(column + 1);
				ar.push_back((machine.buttons[column] >> row) & 1);
			}
		}
		glp_load_matrix(problem, rows * columns, ia.data() - 1, ja.data() - 1, ar.data() - 1);
		glp_iocp iocp;
		glp_init_iocp(&iocp);
		iocp.presolve = GLP_ON;
		iocp.msg_lev = GLP_MSG_ERR;
		glp_intopt(problem, &iocp);
		for (int column = 0; column < columns; ++column)
			total += std::round(glp_mip_col_val(problem, column + 1));
		glp_delete_prob(problem);
	}
	return total;
}

#include "main.h"
