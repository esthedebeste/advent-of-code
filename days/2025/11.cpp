#include <unordered_set>
#include <thread_pool/thread_pool.h>
#include "shared.h"

using Id = uint32_t;
using Input = std::pair<std::unordered_map<Id, std::vector<Id>>, std::unordered_map<Id, std::vector<Id>>>;

static Id read_id(std::string_view &input) {
	Id value = (input[0] << 16) | (input[1] << 8) | input[2];
	input = input.substr(3);
	return value;
}

static Id to_id(std::string_view input) { return read_id(input); }

static std::ostream &print_id(std::ostream &o, Id i) {
	return o << static_cast<char>(i >> 16) << static_cast<char>(i >> 8) << static_cast<char>(i);
}

static Input input(std::string_view input) {
	Input result{};
	auto &[incoming, outgoing] = result;
	auto v = lines([&](std::string_view line) {
		std::pair<Id, std::vector<Id>> result{};
		auto from = read_id(line);
		result.first = from;
		assert_check(line, ": ");
		do {
			auto to = read_id(line);
			result.second.push_back(to);
			incoming[to].push_back(from);
			check(line, ' ');
		} while (!line.empty());
		return result;
	})(input);
	outgoing = {v.begin(), v.end()};
	return result;
}

static std::unordered_set<Id> reachable(const Input &input, const Id start) {
	auto &[incoming, outgoing] = input;
	std::unordered_set<Id> reachable{};
	std::vector<Id> stack{};
	stack.emplace_back(start);
	while (!stack.empty()) {
		Id curr = stack.back();
		stack.pop_back();
		auto [_, inserted] = reachable.insert(curr);
		if (!inserted)
			continue;
		auto it = outgoing.find(curr);
		if (it != outgoing.end())
			for (Id neighbour : it->second) stack.push_back(neighbour);
	}
	return reachable;
}

static int64_t routes(const Input &input, const Id start, const Id goal) {
	auto &[incoming, outgoing] = input;
	auto can_reach = reachable(input, start);
	std::unordered_map<Id, int64_t> incoming_count{};
	for (Id id : can_reach)
		if (auto it = incoming.find(id); it != incoming.end())
			for (Id inc : it->second)
				if (can_reach.contains(inc))
					++incoming_count[id];
	std::vector<Id> stack{};
	stack.push_back(start);
	std::unordered_map<Id, int64_t> total_incoming_count{};
	total_incoming_count[start] = 1;
	while (!stack.empty()) {
		Id curr = stack.back();
		stack.pop_back();
		auto it = outgoing.find(curr);
		if (it != outgoing.end())
			for (Id neighbour : it->second) {
				if (--incoming_count[neighbour] == 0)
					stack.push_back(neighbour);
				total_incoming_count[neighbour] += total_incoming_count[curr];
			}
	}
	auto goal_it = total_incoming_count.find(goal);
	if (goal_it == total_incoming_count.end())
		return 0;
	return goal_it->second;
}

static int part_1(const Input &input) {
	const Id start = to_id("you");
	const Id goal = to_id("out");
	return routes(input, start, goal);
}

static int64_t part_2(const Input &input) {
	const Id start = to_id("svr");
	const Id goal = to_id("out");
	const Id dac = to_id("dac");
	const Id fft = to_id("fft");
	
	auto start_dac = routes(input, start, dac), dac_fft = routes(input, dac, fft), fft_goal = routes(input, fft, goal),
			 start_fft = routes(input, start, fft), fft_dac = routes(input, fft, dac), dac_goal = routes(input, dac, goal);
	return std::max(start_dac * dac_fft * fft_goal, start_fft * fft_dac * dac_goal);
}

#include "main.h"
