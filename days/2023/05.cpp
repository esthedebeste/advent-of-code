#include "shared.h"
#include <execution>

using number_t = uint32_t;

// all inclusive.
struct Range {
	number_t start, end;
	auto operator<=>(const Range &) const = default;
};

std::ostream &operator<<(std::ostream &stream, const Range &range) {
	return stream << range.start << " - " << range.end;
}

// maps from the range from to the range to.
struct Mapping {
	// all inclusive. these ranges have the same length.
	Range from, to;

	Mapping(const Range from, const number_t offset)
		: from(from), to{from.start + offset, from.end + offset} { }

	Mapping(const Range &from, const Range &to)
		: from(from), to(to) { }

	auto operator<=>(const Mapping &) const = default;

	[[nodiscard]] number_t get_mapping(const number_t from) const {
		return from + offset();
	}

	[[nodiscard]] number_t get_inverse_mapping(const number_t to) const {
		return to - offset();
	}

	[[nodiscard]] number_t offset() const { return to.start - from.start; }
	[[nodiscard]] number_t length() const { return from.end - from.start; }
};

std::ostream &operator<<(std::ostream &stream, const Mapping &mapping) {
	return stream << mapping.from << " to " << mapping.to;
}

// assumed to be sorted.
struct Mappings : std::vector<Mapping> {
	[[nodiscard]] number_t get_mapping(number_t from) const {
		for (const auto &mapping : *this) {
			if (from >= mapping.from.start && from <= mapping.from.end) return mapping.get_mapping(from);
		}
		return from;
	}

	[[nodiscard]] number_t get_inverse_mapping(number_t to) const {
		for (const auto &mapping : *this) {
			if (to >= mapping.to.start && to <= mapping.to.end) return mapping.get_inverse_mapping(to);
		}
		return to;
	}

	// unlucky tbh. i didnt expect a loop from 0 up to execute within manageable time. turns out its ~6 seconds. still awful but whatever.

	// [[nodiscard]] Mappings merge(Mappings &layer) {
	// 	for (size_t mapping_i = 0; mapping_i < layer.size(); ++mapping_i) {
	// 		auto &mapping = layer[mapping_i];
	// 		for (size_t overwrite_i = 0, start_size = this->size(); overwrite_i < start_size; ++overwrite_i) {
	// 			auto &existing = (*this)[overwrite_i];
	// 			// if the ranges don't intersect, skip
	// 			if (existing.to.end < mapping.from.start || mapping.from.end < existing.to.start) continue;
	// 			// if the ranges intersect, merge them
	// 			const Range intersect{
	// 				.start = std::max(existing.to.start, mapping.from.start),
	// 				.end = std::min(existing.to.end, mapping.from.end),
	// 			};
	// 			const Range left{existing.to.start, intersect.start - 1}; // .---
	// 			const Range right{intersect.end + 1, existing.to.end}; //   ---.
	// 			if (left.end >= left.start) push_back(Mapping{left, existing.offset()});
	// 			if (right.end >= right.start) push_back(Mapping{right, existing.offset()});
	// 			std::cout << "merging " << existing << " with " << mapping << " to " << left << ", " << intersect << ", " << right << std::endl;
	// 			existing.from = Range{std::min(existing.from.start, mapping.from.start), std::max(existing.from.end, mapping.from.end)};
	// 			existing.to = Range{intersect.start + mapping.offset(), intersect.end + mapping.offset()};
	// 			std::cout << "  to " << existing << std::endl;
	// 		}
	// 	}
	// 	std::ranges::sort(*this, {}, [](const Mapping &m) { return m.from.start; });
	// 	Mappings uniques({1, this->front()});
	// 	for (size_t i = 1; i < this->size(); ++i) {
	// 		const auto &mapping = this->at(i);
	// 		const auto &last = uniques.back();
	// 		if (last.from.start == mapping.from.start && last.from.end == mapping.from.end &&
	// 			last.to.start == mapping.to.start && last.to.end == mapping.to.end)
	// 			continue;
	// 		uniques.push_back(mapping);
	// 	}
	// 	return uniques;
	// }
};

struct MappingLevels : std::vector<Mappings> {
	[[nodiscard]] number_t get_mapping(number_t from) const {
		for (const auto &level : *this) from = level.get_mapping(from);
		return from;
	}

	[[nodiscard]] number_t get_inverse_mapping(number_t to) const {
		for (const auto &level : *this | std::views::reverse) to = level.get_inverse_mapping(to);
		return to;
	}
};

struct Input {
	std::vector<number_t> seeds;
	MappingLevels mappings;
};

int main() {
	day(
		[](std::istream &file) {
			Input input;
			auto &[seeds,mappings] = input;
			file >> "seeds: ";
			do {
				number_t seed;
				file >> seed;
				seeds.push_back(seed);
			} while (file.get() != '\n');
			// all-inclusive mappings
			std::cout << "mappings: " << mappings.size() << std::endl;
			do {
				file >> "\n" >> skip_until('\n');
				Mappings &new_mappings = mappings.emplace_back();
				do {
					number_t to_start, start, length;
					file >> to_start >> " " >> start >> " " >> length;
					new_mappings.emplace_back(Range{start, start + length - 1}, Range{to_start, to_start + length - 1});
					if (file.peek() == EOF) return input;
					file >> "\n";
				} while (file.peek() != '\n');
				std::ranges::sort(new_mappings, {}, [](const Mapping &m) { return m.from.start; });
			} while (!file.eof());
			unreachable();
		}, [](const Input &input) {
			number_t least = std::numeric_limits<number_t>::max();
			for (const auto seed : input.seeds) {
				const number_t res = input.mappings.get_mapping(seed);
				if (res < least) least = res;
			}
			return least;
		}, [](const Input &input) {
			for (number_t location = 0; location < std::numeric_limits<number_t>::max(); ++location) {
				auto target_seed = input.mappings.get_inverse_mapping(location);
				for (int i = 0; i < input.seeds.size(); i += 2) {
					auto seed_min = input.seeds[i];
					auto seed_max = seed_min + input.seeds[i + 1] - 1;
					if (target_seed >= seed_min && target_seed <= seed_max) {
						return location;
					}
				}
			};
		});
}
