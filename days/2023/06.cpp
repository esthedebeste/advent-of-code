#include "shared.h"
#include <execution>

struct Race {
	int64_t ms = 0, mm = 0;
};

int main() {
	using input = std::pair<std::vector<Race>, Race>;
	day(
		[](std::string_view file) {
			input inp;
			auto &[races, big_race] = inp;
			std::vector<uint16_t> little_reused_temp_vector;
			file.remove_prefix(sizeof("Time:    ") - 1);
			do {
				while (file.front() == ' ') { file.remove_prefix(1); }
				auto &race = races.emplace_back();
				do {
					race.ms = race.ms * 10 + file.front() - '0';
					big_race.ms = big_race.ms * 10 + file.front() - '0';
					file.remove_prefix(1);
				} while (isdigit(file.front()));
			} while (file.front() != '\n');
			file.remove_prefix(sizeof("\nDistance:") - 1);
			for (auto &race : races) {
				while (file.front() == ' ') { file.remove_prefix(1); }
				do {
					race.mm = race.mm * 10 + file.front() - '0';
					big_race.mm = big_race.mm * 10 + file.front() - '0';
					file.remove_prefix(1);
				} while (file.size() > 0 && isdigit(file.front()));
			}
			return inp;
		},
		[](const input &input) {
			int total = 1;
			for (const auto [ms, mm] : input.first) {
				int solutions = 0;
				for (int button_time = 1; button_time < ms; ++button_time) {
					const auto time_to_move = ms - button_time;
					const auto distance_moved = time_to_move * button_time;
					if (distance_moved > mm) {
						solutions++;
					}
				}
				total *= solutions;
			}
			return total;
		},
		[](const input &input) {
			int solutions = 0;
			const auto [ms, mm] = input.second;
			for (int button_time = 1; button_time < ms; ++button_time) {
				const auto time_to_move = ms - button_time;
				const auto distance_moved = time_to_move * button_time;
				if (distance_moved > mm) {
					solutions++;
				}
			}
			return solutions;
		});
}
