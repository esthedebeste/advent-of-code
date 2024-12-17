
#include "shared.h"

using reg_t = uint64_t;
struct Program {
	reg_t a, b, c;
	std::vector<uint8_t> data;
	size_t run(const auto &output) {
		size_t instruction_ptr = 0;
		size_t print_count = 0;
		while (instruction_ptr < data.size()) {
			const auto ins = data[instruction_ptr];
			const auto literal = data[instruction_ptr + 1];
			reg_t combo;
			switch (literal) {
			case 0:
			case 1:
			case 2:
			case 3:
				combo = literal;
				break;
			case 4:
				combo = a;
				break;
			case 5:
				combo = b;
				break;
			case 6:
				combo = c;
				break;
			case 7:
			default:
				unreachable();
			}
			instruction_ptr += 2;
			switch (ins) {
			case 0:
				// ADV
				a = a >> combo;
				break;
			case 1:
				// BXL
				b = b ^ literal;
				break;
			case 2:
				// BST
				b = combo & 0b111;
				break;
			case 3:
				// JNZ
				if (a != 0)
					instruction_ptr = literal;
				break;
			case 4:
				// BXC
				b = b ^ c;
				break;
			case 5:
				// OUT
				if (!output(combo & 0b111, print_count++))
					return 0;
				break;
			case 6:
				// BDV
				b = a >> combo;
				break;
			case 7:
				// CDV
				c = a >> combo;
				break;
			default:
				unreachable();
			}
		}
		return print_count;
	}
};

int main() {
	day(
		[](std::istream &is) {
			Program program;
			is >> "Register A: " >> program.a >> "\nRegister B: " >> program.b >> "\nRegister C: " >> program.c >> "\n\nProgram: ";
			do {
				program.data.emplace_back(parse<int>(is));
			} while (check(is, ","));
			return program;
		},
		[](Program program) {
			std::string str{};
			program.run([&](const int value, const size_t index) {
				if (index != 0)
					str += ",";
				str += std::to_string(value);
				return true;
			});
			return str;
		},
		[](Program program) {
			// the input is a little loop, looking something like:
			// do {
			//	B = (something ^ (A%8) ^ (A >> (A%8 ^ something)))
			//	print(B)
			//	A = A >> 3
			// } while(A != 0)
			// Because the shift in the B calculation is rightward, we can start at the most
			// significant 3 bits of A, and work our way to the right, to figure out the
			// necessary starting value for A.
			// Some backtracking is required, because earlier prints are dependent on later prints
			// (because of the A >> something).
			auto &[a, b, c, data] = program;
			std::vector<uint8_t> a_3bit_portions(data.size(), 255);
			reg_t last_a = 0;
			for (size_t i = 0; i < data.size(); i++) {
				auto print_i = data.size() - i - 1;
				for (uint8_t trying = a_3bit_portions[i] + 1; trying < 8; ++trying) {
					a_3bit_portions[i] = trying;
					last_a = 0;
					for (const uint8_t k : a_3bit_portions) last_a = (last_a << 3) | (k & 0b111);
					a = last_a;
					b = c = 0;
					if (data.size() == program.run([&](const uint8_t value, const size_t index) {
								if (index >= data.size())
									return false;
								if (index >= print_i)
									return data[index] == value;
								return true;
							}))
						goto success;
				}
				// no success :(
				a_3bit_portions[i] = 255; // reset
				i -= 2; // backtrack!
			success:
			}
			return last_a;
		});
}
