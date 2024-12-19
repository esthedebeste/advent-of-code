
#include <bitset>


#include "shared.h"

using dpt = pos_t<>::member_type;
constexpr dpt SIZE = 71;
constexpr pos_t END{SIZE - 1, SIZE - 1};

int main() {
	day(
		lines([](std::istream &is) {
			pos_t pos{};
			is >> pos.x >> ',' >> pos.y;
			return pos;
		}),
		[](const std::vector<pos_t<>> &falls) {
			std::bitset<SIZE * SIZE> fallen{};
			fallen.reset();
			for (const auto &fall : falls | std::views::take(1024)) fallen.set(fall.to_1d(SIZE));
			return dijkstra(
				pos_t{0, 0}, [](const pos_t<> &pos) { return pos == END; }, [](const pos_t<> &pos) { return pos.neighbours(); },
				[&fallen](const pos_t<> &prev, const pos_t<> &neighbour) {
					return neighbour.x >= 0 && neighbour.y >= 0 && neighbour.x < SIZE && neighbour.y < SIZE && !fallen[neighbour.to_1d(SIZE)];
				},
				[](const pos_t<> &prev, const pos_t<> &neighbour) { return 1; });
		},
		[](const std::vector<pos_t<>> &falls) {
			std::bitset<SIZE * SIZE> fallen{};
			fallen.reset();
			for (const auto &fall : falls | std::views::take(1024)) fallen.set(fall.to_1d(SIZE));
			for (const auto &fall : falls | std::views::drop(1024)) {
				fallen.set(fall.to_1d(SIZE));
				// could be optimized by not recomputing until a blockade has been dropped onto the previously used path
				if (-1 ==
						dijkstra(
							pos_t{0, 0}, [](const pos_t<> &pos) { return pos == END; }, [](const pos_t<> &pos) { return pos.neighbours(); },
							[&fallen](const pos_t<> &prev, const pos_t<> &neighbour) {
								return neighbour.x >= 0 && neighbour.y >= 0 && neighbour.x < SIZE && neighbour.y < SIZE && !fallen[neighbour.to_1d(SIZE)];
							},
							[](const pos_t<> &prev, const pos_t<> &neighbour) { return 1; }))
					return fall;
			}
		});
}
