#pragma once
#include <cassert>
#include <istream>
#include <limits>
#include <queue>
#include <unordered_map>
#include <utility>

#if __cpp_lib_unreachable >= 202202L
#define unreachable() std::unreachable() // C++23
#elif defined(__GNUC__)
#define unreachable() __builtin_unreachable(); // GCC and Clang
#elif defined(_MSC_VER)
#define unreachable() __assume(false); // MSVC
#endif
inline constexpr auto max(auto first, auto... args) {
	auto curr = first;
	(..., (curr = curr > args ? curr : args));
	return curr;
}

inline constexpr auto min(auto first, auto... args) {
	auto curr = first;
	(..., (curr = curr < args ? curr : args));
	return curr;
}

/// usage: input >> "Index: " >> index;
/// skip a string and throw (unreachable) if it doesn't match
template <size_t Len>
std::istream &operator>>(std::istream &in, const char (&s)[Len]) {
	for (size_t i = 0; i < Len - 1; i++) // -1 to skip the null terminator
		if (in.get() != s[i]) {
			unreachable();
			exit(1);
		}
	return in;
}

/// usage: check(input, "Index: ");
/// skip a string and return false if it doesn't match
template <size_t Len>
bool check(std::istream &in, const char (&s)[Len]) {
	const auto start = in.tellg();
	for (size_t i = 0; i < Len - 1; i++) {
		// Len - 1 to skip the null terminator
		if (in.get() != s[i]) {
			in.seekg(start);
			return false;
		}
	}
	return true;
}

template <typename Matcher> concept CheckMatcher = requires(Matcher m, char c) {
	{ m(c) } -> std::convertible_to<bool>;
};

/// usage: check(input, isdigit);
/// skip a character and return false if it doesn't match
bool check(std::istream &in, CheckMatcher auto matcher) {
	const bool result = matcher(in.peek());
	if (result) in.ignore(1);
	return result;
}

struct skip_until {
	char c;

	constexpr skip_until(char c)
		: c{c} { }
};

std::istream &operator>>(std::istream &in, skip_until s) {
	in.ignore(std::numeric_limits<std::streamsize>::max(), s.c);
	return in;
}

template <typename T1, typename T2> struct std::hash<std::pair<T1, T2>> {
	size_t operator()(const pair<T1, T2> &p) const {
		return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
	}
};

#ifndef default_pos_t
#define default_pos_t int
#endif

template <std::integral T = default_pos_t> struct pos_t {
	T x, y;

	pos_t<T> up() const {
#if pos_up == 0
		return {x, y - 1};
#elif pos_up == 1
    return {x, y + 1};
#endif
	}

	pos_t<T> down() const {
#if pos_up == 0
		return {x, y + 1};
#elif pos_up == 1
    return {x, y - 1};
#endif
	}

	pos_t<T> left() const { return {x - 1, y}; }
	pos_t<T> right() const { return {x + 1, y}; }

	T manhattan_distance(pos_t<T> other) const {
		return abs(x - other.x) + abs(y - other.y);
	}

	friend bool operator==(pos_t<T> a, pos_t<T> b) {
		return a.x == b.x && a.y == b.y;
	}

	friend bool operator<(pos_t<T> a, pos_t<T> b) {
		return a.x < b.x || (a.x == b.x && a.y < b.y);
	}

	friend pos_t<T> operator+(pos_t<T> a, pos_t<T> b) {
		return {a.x + b.x, a.y + b.y};
	}

	friend pos_t<T> operator-(pos_t<T> a, pos_t<T> b) {
		return {a.x - b.x, a.y - b.y};
	}
};

namespace std {
	template <std::integral T> struct hash<pos_t<T>> {
		size_t operator()(const pos_t<T> &x) const {
			return hash<T>()(x.x) ^ hash<T>()(x.y);
		}
	};
}

using pos = pos_t<default_pos_t>;

template <typename T>
int dijkstra(
	T start, T goal, auto neighbours, auto cost = [](T, T) { return 1; }) {
	using elem = std::pair<int, T>;
	std::priority_queue<elem, std::vector<elem>, std::greater<elem>> queue;
	std::unordered_map<T, T> chain;
	std::unordered_map<T, int> cost_so_far;

	queue.emplace(0, start);
	chain[start] = start;
	cost_so_far[start] = 0;

	while (!queue.empty()) {
		T current = queue.top().second;
		queue.pop();

		if (current == goal) break; // found!

		for (T next : neighbours(current)) {
			int new_cost = cost_so_far[current] + 1;
			if (!cost_so_far.contains(next) || new_cost < cost_so_far[next]) {
				cost_so_far[next] = new_cost;
				chain[next] = current;
				queue.emplace(new_cost, next);
			}
		}
	}

	int len = 0;
	T current = goal;
	if (!chain.contains(goal)) return -1; // no path found :(
	while (current != start) {
		len++;
		current = chain[current];
	}
	return len;
}

template <typename T> auto dijkstra(T start, T goal, auto neighbours) {
	return dijkstra(start, goal, neighbours, [](T, T) { return 1; });
}

namespace utils {
	constexpr bool isdigit(const char c) { return c >= '0' && c <= '9'; }
}

using utils::isdigit;

pos retrace_colrow(std::istream &istream) {
	auto until = istream.tellg();
	istream.seekg(0);
	pos pos{0, 1};
	for (char c; istream.tellg() != until;) {
		istream.get(c);
		if (c == '\n') {
			pos.x = 0;
			pos.y++;
		} else {
			pos.x++;
		}
	}
	return pos;
}
