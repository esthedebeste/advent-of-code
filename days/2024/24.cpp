#include <set>
#include "shared.h"

using Operand = uint32_t;
Operand name(const char c, const uint8_t number) {
	return (static_cast<uint32_t>(c) << 16) | (static_cast<uint32_t>(number / 10 + '0') << 8) | static_cast<uint32_t>(number % 10 + '0');
}
Operand name(const char c[4]) {
	return (static_cast<uint32_t>(c[0]) << 16) | (static_cast<uint32_t>(c[1]) << 8) | static_cast<uint32_t>(c[2]);
}
std::string operand_str(const Operand operand) {
	char str[4];
	str[0] = static_cast<char>(operand >> 16);
	str[1] = static_cast<char>(operand >> 8);
	str[2] = static_cast<char>(operand);
	str[3] = '\0';
	return std::string(str);
}
enum Operator : uint8_t { OR, AND, XOR, ALWAYS_TRUE, ALWAYS_FALSE };
struct Operation;
using World = std::unordered_map<Operand, Operation>;
struct Operation {
	Operator op;
	std::pair<Operand, Operand> operands;
	Operation(const Operator op, const Operand left, const Operand right) : op(op), operands(std::minmax(left, right)) {}
	explicit Operation(const bool always) : op(always ? ALWAYS_TRUE : ALWAYS_FALSE) {}
	auto operator<=>(const Operation &) const = default;

	[[nodiscard]] bool is_const() const { return op == ALWAYS_TRUE || op == ALWAYS_FALSE; }
	[[nodiscard]] bool calculate(const World &world) const {
		const auto &left = operands.first;
		const auto &right = operands.second;
		switch (op) {
		case OR:
			return world.at(left).calculate(world) || world.at(right).calculate(world);
		case AND:
			return world.at(left).calculate(world) && world.at(right).calculate(world);
		case XOR:
			return world.at(left).calculate(world) ^ world.at(right).calculate(world);
		case ALWAYS_TRUE:
			return true;
		case ALWAYS_FALSE:
			return false;
		default:
			unreachable();
		}
	}
};


template <> struct std::hash<Operation> {
	size_t operator()(const Operation &o) const noexcept {
		return std::hash<uint8_t>{}(o.op) ^ std::hash<std::pair<Operand, Operand>>{}(o.operands);
	}
};

int main() {
	uint8_t highest_z_bit = 0;
	day(
		[](std::istream &is) -> World {
			World world{};
			do {
				char constant[4];
				char value;
				is >> constant >> ": " >> value;
				world.emplace(name(constant), Operation(value == '1'));
			} while (check(is, '\n') && is.peek() != '\n');
			is >> '\n';
			do {
				char left[4], right[4], to[4];
				char op_first;
				is >> left >> " " >> op_first >> skip_until(' ') >> right >> " -> " >> to;
				Operator op;
				switch (op_first) {
				case 'O':
					op = OR;
					break;
				case 'A':
					op = AND;
					break;
				case 'X':
					op = XOR;
					break;
				default:
					unreachable();
				}
				world.emplace(name(to), Operation(op, name(left), name(right)));
			} while (check(is, '\n') && std::isalpha(is.peek()));
			return world;
		},
		[&](const World &world) {
			for (int i = 0;; i++) {
				if (world.contains(name('z', i)))
					highest_z_bit = i;
				else
					break;
			}
			uint64_t z = 0;
			for (uint8_t i = 0; i <= highest_z_bit; ++i)
				if (world.at(name('z', i)).calculate(world))
					z |= 1ULL << i;
			return z;
		},
		[&](const World &world) {
			std::unordered_map<Operation, Operand> operation_name{};
			std::unordered_map<std::pair<Operator, Operand>, Operand> other_operand{};
			for (const auto &[name, operation] : world) {
				operation_name.emplace(operation, name);
				if (operation.is_const())
					continue; // operands don't exist
				other_operand.emplace(std::make_pair(operation.op, operation.operands.first), operation.operands.second);
				other_operand.emplace(std::make_pair(operation.op, operation.operands.second), operation.operands.first);
			}

			std::set<Operand> swaps{};
			Operand in_c = operation_name.at({AND, name('x', 0), name('y', 0)});
			// aoc is very very very very nice to us and does not put more than one problem in any given adder block
			for (uint8_t i = 1; i < highest_z_bit; ++i) {
				const auto &x_name = name('x', i);
				const auto &y_name = name('y', i);
				const auto &z_name = name('z', i);
				// these will always work because x and y are constants
				auto xor_1 = operation_name.at({XOR, x_name, y_name});
				auto and_1 = operation_name.at({AND, x_name, y_name});
				// these we're not sure about - xor_1 and in_c are uncertain
				auto xor_2 = operation_name.find(Operation{XOR, in_c, xor_1});
				auto and_2 = operation_name.find(Operation{AND, in_c, xor_1});
				if (xor_2 == operation_name.end()) {
					if (auto in_c_into_and_2 = other_operand.find({AND, xor_1}); in_c_into_and_2 != other_operand.end()) {
						// in_c is the problem, xor_1 is right.
						// so swap in_c, and the in_c we can find through and_2
						swaps.insert(in_c);
						swaps.insert(in_c_into_and_2->second);
						in_c = in_c_into_and_2->second;
					} else {
						// xor_1 is the problem, in_c is right.
						auto xor_1_into_and_2 = other_operand.at({AND, in_c});
						// so swap xor, and the xor we can find through and_2
						swaps.insert(xor_1);
						swaps.insert(xor_1_into_and_2);
						xor_1 = xor_1_into_and_2;
					}
					// refresh after having changed either in_c or xor_1
					xor_2 = operation_name.find(Operation{XOR, in_c, xor_1});
					and_2 = operation_name.find(Operation{AND, in_c, xor_1});
				}
				if (xor_2->second != z_name) {
					// the output of xor2 needs to be z__, otherwise just swap the two, and we're good :)
					swaps.insert(xor_2->second);
					swaps.insert(z_name);
				}
				auto or_g = operation_name.find(Operation{OR, and_1, and_2->second});
				if (or_g == operation_name.end()) {
					// if the carry OR can't be found, and_1 or and_2 is mislinked
					if (auto and_2_into_carry = other_operand.find({OR, and_1}); and_2_into_carry != other_operand.end()) {
						// and_2 is the problem, and_1 is right.
						// so swap and2, and the and2 we can find through the carry
						swaps.insert(and_2_into_carry->second);
						swaps.insert(and_2->second);
						and_2->second = and_2_into_carry->second;
					} else {
						auto and_1_into_carry = other_operand.at({OR, and_2->second});
						// and_1 is the problem, and_2 is right.
						// so swap and1, and the and1 we can find through the carry
						swaps.insert(and_1_into_carry);
						swaps.insert(and_1);
						and_1 = and_1_into_carry;
					}
					// refresh the OR gate after changing either and_1 or and_2
					or_g = operation_name.find(Operation{OR, and_1, and_2->second});
				}
				// the next bit's in_c is this bit's counter
				in_c = or_g->second;
			}
			if (const auto &last_z_name = name('z', highest_z_bit); last_z_name != in_c) {
				// the final carry isn't in the highest z, swap them
				swaps.insert(in_c);
				swaps.insert(last_z_name);
			}
			// little printing step to get it formatted perfectly
			return print_fun([swaps](std::ostream &out) {
				for (const auto &[i, s] : std::views::enumerate(swaps)) {
					if (i != 0)
						out << ',';
					out << operand_str(s);
				}
			});
		});
}
