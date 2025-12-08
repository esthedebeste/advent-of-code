#include "shared.h"

struct JunctionBox : std::tuple<int, int, int> {
	using std::tuple<int, int, int>::tuple;
	int &x() { return std::get<0>(*this); }
	int &y() { return std::get<1>(*this); }
	int &z() { return std::get<2>(*this); }
	const int &x() const { return std::get<0>(*this); }
	const int &y() const { return std::get<1>(*this); }
	const int &z() const { return std::get<2>(*this); }
};

using Edge = std::tuple<size_t, size_t, uint64_t>;
using Input = std::pair<std::vector<JunctionBox>, std::vector<Edge>>;

Input input(std::string_view input) {
	Input result{};
	auto &[boxes, edges] = result;
	boxes = lines([](std::string_view line) {
		size_t first_comma = line.find(',');
		size_t second_comma = line.find(',', first_comma + 1);
		auto x = line.substr(0, first_comma), y = line.substr(first_comma + 1, second_comma - first_comma), z = line.substr(second_comma + 1);
		return JunctionBox{parse_num(x), parse_num(y), parse_num(z)};
	})(input);
	// there are probably faster ways to find the n shortest edges than finding all edges and then sorting them
	for (auto i = 0; i < boxes.size(); ++i) {
		auto &a = boxes[i];
		for (auto j = i + 1; j < boxes.size(); ++j) {
			auto &b = boxes[j];
			int64_t x = b.x() - a.x(), y = b.y() - a.y(), z = b.z() - a.z();
			edges.emplace_back(i, j, x * x + y * y + z * z);
		}
	}
	std::sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b) { return std::get<2>(a) < std::get<2>(b); });
	return result;
}

uint64_t part_1(const Input &input) {
	auto &[boxes, edges] = input;
	union_find uf{boxes.size()};
	for (int i = 0; i < 1000; ++i) {
		auto &[a, b, l] = edges[i];
		uf.merge(a, b);
	}
	auto size = std::vector<size_t>(boxes.size(), 0);
	for (size_t i = 0; i < boxes.size(); ++i) {
		size[uf.find(i)] += 1;
	}
	std::sort(size.rbegin(), size.rend());
	return size[0] * size[1] * size[2];
}

uint64_t part_2(const Input &input) {
	auto &[boxes, edges] = input;
	union_find uf{boxes.size()};

	int total_merged = 1;
	for (size_t curr_edge = 0; curr_edge < edges.size(); ++curr_edge) {
		auto &[a, b, l] = edges[curr_edge];
		if (!uf.merge(a, b))
			continue;
		++total_merged;
		if (total_merged == boxes.size())
			return static_cast<uint64_t>(boxes[a].x()) * boxes[b].x();
	}
	return 0;
}

#include "main.h"
