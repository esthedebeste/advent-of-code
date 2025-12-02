#include <random>
#include <thread>
#include "shared.h"


int main() {
	day(
		lines([](std::istream &input) {
			char side;
			input >> side;
			int amount;
			input >> amount;
			if (side == 'L')
				amount = -amount;
			return amount;
		}),
		[](const std::vector<int> &s) {
			size_t zeroes = 0;
			int curr = 50;
			for (int i : s) {
				curr = ((curr + i) + 100) % 100;
				if (curr == 0)
					zeroes++;
			}
			return zeroes;
		},
		[](const std::vector<int> &s) {
			size_t zeroes = 0;
			int curr = 50;
			for (int i : s) {
				int sig = i >= 0 ? 1 : -1;
				for (int x = 0; x < std::abs(i); x++) {
					curr = ((curr + sig) + 100) % 100; // click!
					if (curr == 0) // if this click results in 0
						zeroes++; // increment
				}
			}
			return zeroes;
		});
}
