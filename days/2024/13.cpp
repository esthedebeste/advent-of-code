#define default_pos_t uint64_t
#include "shared.h"
#include <gmp.h>
#include <mpfr.h>

using dpt = pos_t<>::member_type;

struct Machine {
	pos_t<> button_a, button_b;
	pos_t<> prize;
};

std::ostream &operator<<(std::ostream &stream, const Machine &machine) {
	return stream << "(A: " << machine.button_a << ", B: " << machine.button_b << ", P: " << machine.prize << ")";
}

// Floating point precision to use. This is the lowest power of two that produces the correct result :)
constexpr auto FPC = 2048;
void get_necessary_tokens(const Machine &machine, mpfr_t& add_to) {
	// https://www.desmos.com/calculator/hrodtzzr6a
	mpfr_t ax, ay, bx, by, gx, gy, a, b, d, tmp, tmp2, tmp3, answer;
	mpfr_init2(ax, FPC), mpfr_init2(ay, FPC), mpfr_init2(bx, FPC), mpfr_init2(by, FPC), mpfr_init2(gx, FPC), mpfr_init2(gy, FPC),
		mpfr_init2(a, FPC), mpfr_init2(b, FPC), mpfr_init2(d, FPC), mpfr_init2(tmp, FPC), mpfr_init2(tmp2, FPC), mpfr_init2(tmp3, FPC), mpfr_init2(answer, FPC);
	mpfr_set_uj(ax, machine.button_a.x, MPFR_RNDN); // ax = ax
	mpfr_set_uj(ay, machine.button_a.y, MPFR_RNDN); // ay = ay
	mpfr_set_uj(bx, machine.button_b.x, MPFR_RNDN); // bx = bx
	mpfr_set_uj(by, machine.button_b.y, MPFR_RNDN); // by = by
	mpfr_set_uj(gx, machine.prize.x, MPFR_RNDN); // gx = gx
	mpfr_set_uj(gy, machine.prize.y, MPFR_RNDN); // gy = gy
	mpfr_div(a, ay, ax, MPFR_RNDN); // a = ay/ax
	mpfr_div(b, by, bx, MPFR_RNDN); // b = by/bx
	// mpfr_fprintf(stderr, "ax %.2RDf\nay %.2RDf\nbx %.2RDf\nby %.2RDf\ngx %.2RDf\ngy %.2RDf\na %.2RDf\nb %.2RDf\n", ax, ay, bx, by, gx, gy, a, b);
	mpfr_mul(d, b, gx, MPFR_RNDN); // d = b * gx
	mpfr_sub(d, gy, d, MPFR_RNDN); // d = gy - d
	mpfr_sub(tmp, a, b, MPFR_RNDN); // tmp = a - b
	mpfr_div(d, d, tmp, MPFR_RNDN); // d = d / tmp
	mpfr_set_ui(answer, 3, MPFR_RNDN); // answer = 3
	mpfr_mul(answer, answer, d, MPFR_RNDN); // answer = 3 * d
	mpfr_div(answer, answer, ax, MPFR_RNDN); // answer = 3 * d / ax
	mpfr_sub(tmp, gx, d, MPFR_RNDN); // tmp = gx - d
	mpfr_div(tmp, tmp, bx, MPFR_RNDN); // tmp = tmp / bx
	mpfr_add(answer, answer, tmp, MPFR_RNDN); // answer = 3 * d / ax + (gx - d) / bx
	// mpfr_fprintf(stderr, "hi d %.100RDf\n a %.100RDf\n", d, answer);
	mpfr_fmod(tmp, d, ax, MPFR_RNDN); // tmp = d % ax
	mpfr_abs(tmp, tmp, MPFR_RNDN); // tmp = abs(tmp)
	mpfr_set_ld(tmp2, std::numeric_limits<long double>::min(), MPFR_RNDN);
	bool d_round = mpfr_lessequal_p(tmp, tmp2) != 0; // tmp <= (small)
	if (!d_round) {
		mpfr_sub(tmp2, ax, tmp2, MPFR_RNDN);
		d_round = mpfr_greaterequal_p(tmp, tmp2) != 0; // || tmp >= 1 - (small)
	}
	// mpfr_fprintf(stderr, "frac %.100RDf zero=%hhd\n", tmp, dld_zero);
	if (d_round)
		mpfr_add(add_to, add_to, answer, MPFR_RNDN); // total += answer
	mpfr_clear(ax), mpfr_clear(ay), mpfr_clear(bx), mpfr_clear(by), mpfr_clear(gx), mpfr_clear(gy), mpfr_clear(a), mpfr_clear(b),
	mpfr_clear(d), mpfr_clear(tmp), mpfr_clear(tmp2), mpfr_clear(tmp3), mpfr_clear(answer);
}

int main() {
	day(
		[](std::istream &is) {
			std::vector<Machine> machines;
			do {
				auto &[button_a, button_b, prize] = machines.emplace_back();
				is >> "Button A: X" >> button_a.x >> ", Y" >> button_a.y >> "\nButton B: X" >> button_b.x >> ", Y" >> button_b.y >> "\nPrize: X=" >>
					prize.x >> ", Y=" >> prize.y;
			} while (check(is, "\n\n"));
			return machines;
		},
		[](const std::vector<Machine> &machines) {
			mpfr_t total;
			mpfr_init2(total, FPC);
			mpfr_set_zero(total, 1);
			for (const auto &[mi, machine] : std::views::enumerate(machines))
				get_necessary_tokens(machine, total);
			const uintmax_t total_uj = mpfr_get_uj(total, MPFR_RNDN);
			mpfr_clear(total);
			return total_uj;
		},
		[](std::vector<Machine> &machines) {
			mpfr_t total;
			mpfr_init2(total, FPC);
			mpfr_set_zero(total, 1);
			for (const auto &[mi, machine] : std::views::enumerate(machines)) {
				machine.prize.x += 10000000000000;
				machine.prize.y += 10000000000000;
				get_necessary_tokens(machine, total);
			}
			const uintmax_t total_uj = mpfr_get_uj(total, MPFR_RNDN);
			mpfr_clear(total);
			return total_uj;
		});
}
