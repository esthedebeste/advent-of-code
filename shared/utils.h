#pragma once
#include <cassert>
#include <cstdint>
#include <functional>
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
#ifdef _MSC_VER
#define really_always_inline __forceinline
#else
#define really_always_inline inline __attribute__((always_inline))
#endif
constexpr auto max(auto first, auto... args) {
	auto curr = first;
	(..., (curr > args ? curr : curr = args));
	return curr;
}

constexpr auto min(auto first, auto... args) {
	auto curr = first;
	(..., (curr < args ? curr : curr = args));
	return curr;
}

/// usage: input >> "Index: " >> index;
/// skip a string and throw (unreachable) if it doesn't match
template <size_t Len> std::istream &operator>>(std::istream &in, const char (&s)[Len]) {
	for (size_t i = 0; i < Len - 1; i++) // -1 to skip the null terminator
		if (in.get() != s[i]) {
			unreachable(), std::exit(1);
		}
	return in;
}

/// usage: check(input, "Index: ");
/// skip a string and return false if it doesn't match
template <size_t Len> bool check(std::istream &in, const char (&s)[Len]) {
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

/// usage: check(input, "Index: ");
/// skip a string and return false if it doesn't match
template <size_t Len> bool check(std::string_view &in, const char (&s)[Len]) {
	if (in.length() < Len - 1)
		return false;
	for (size_t i = 0; i < Len - 1; i++)
		// Len - 1 to skip the null terminator
		if (in[i] != s[i])
			return false;
	in = in.substr(Len - 1);
	return true;
}

template <typename Matcher>
concept CheckMatcher = requires(Matcher m, char c) {
	{ m(c) } -> std::convertible_to<bool>;
};

/// usage: check(input, isdigit);
/// skip a character and return false if it doesn't match
bool check(std::istream &in, CheckMatcher auto matcher) {
	const bool result = matcher(in.peek());
	if (result)
		in.ignore(1);
	return result;
}

/// usage: check(input, isdigit);
/// skip a character and return false if it doesn't match
bool check(std::string_view &in, CheckMatcher auto matcher) {
	if (in.empty())
		return false;
	const bool result = matcher(in[0]);
	if (result)
		in = in.substr(1);
	return result;
}

/// usage: check(input, 'a');
/// skip a character and return false if it doesn't match
inline bool check(std::istream &in, const char character) {
	const bool result = in.peek() == character;
	if (result)
		in.ignore(1);
	return result;
}

/// usage: check(input, isdigit);
/// skip a character and return false if it doesn't match
inline bool check(std::string_view &in, const char character) {
	if (in.empty())
		return false;
	const bool result = in[0] == character;
	if (result)
		in = in.substr(1);
	return result;
}

/// use as you would `check`, but this asserts that it must match
template <class I, class C> inline void assert_check(I &in, C c) {
	bool passed = check(in, c);
	assert(passed);
}

/// usage: check(input, "Index: ");
/// skip a string and assert that it does match
template <class I, size_t Len> inline void assert_check(I &in, const char (&s)[Len]) {
	bool passed = check(in, s);
	assert(passed);
}

struct skip_until {
	char c;

	constexpr skip_until(const char c) : c{c} {}
};

inline std::istream &operator>>(std::istream &in, const skip_until s) {
	in.ignore(std::numeric_limits<std::streamsize>::max(), s.c);
	return in;
}

template <class G> struct print_fun {
	G fun;

	explicit constexpr print_fun(G fun) : fun{fun} {}
};

template <class G> std::ostream &operator<<(std::ostream &in, const print_fun<G> &f) {
	f.fun(in);
	return in;
}

template <typename T1, typename T2> struct std::hash<std::pair<T1, T2>> {
	size_t operator()(const pair<T1, T2> &p) const { return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second); }
};

#ifndef default_pos_t
#define default_pos_t int
#endif

template <std::integral T = default_pos_t> struct pos_t {
	using member_type = T;
	T x, y;

	[[nodiscard]]
	pos_t<T> up() const {
#if pos_up == 0
		return {x, static_cast<T>(y - 1)};
#elif pos_up == 1
		return {x, static_cast<T>(y + 1)};
#endif
	}

	[[nodiscard]]
	pos_t<T> down() const {
#if pos_up == 0
		return {x, static_cast<T>(y + 1)};
#elif pos_up == 1
		return {x, static_cast<T>(y - 1)};
#endif
	}

	[[nodiscard]]
	pos_t<T> left() const {
		return {static_cast<T>(x - 1), y};
	}
	[[nodiscard]]
	pos_t<T> right() const {
		return {static_cast<T>(x + 1), y};
	}

	[[nodiscard]]
	std::array<pos_t, 4> neighbours() const {
		return {up(), right(), down(), left()};
	}

	[[nodiscard]]
	pos_t<T> rotate_left_90() const {
#if pos_up == 0
		return {y, -x};
#elif pos_up == 1
		return {-y, x};
#endif
	}

	[[nodiscard]]
	pos_t<T> rotate_right_90() const {
#if pos_up == 0
		return {-y, x};
#elif pos_up == 1
		return {y, -x};
#endif
	}

	[[nodiscard]]
	T manhattan_distance(pos_t<T> other) const {
		return abs(x - other.x) + abs(y - other.y);
	}

	[[nodiscard]]
	T to_1d(T width) const {
		return y * width + x;
	}

	[[nodiscard]]
	static pos_t<T> from_1d(T width, T index) {
		return {index % width, index / width};
	}

	friend bool operator==(pos_t<T> a, pos_t<T> b) { return a.x == b.x && a.y == b.y; }

	friend bool operator<(pos_t<T> a, pos_t<T> b) { return a.x < b.x || (a.x == b.x && a.y < b.y); }

	friend pos_t<T> operator+(pos_t<T> a, pos_t<T> b) { return {a.x + b.x, a.y + b.y}; }

	friend pos_t<T> operator-(pos_t<T> a, pos_t<T> b) { return {a.x - b.x, a.y - b.y}; }

	friend pos_t<T> operator*(pos_t<T> pos, T scale) { return {pos.x * scale, pos.y * scale}; }

	friend std::ostream &operator<<(std::ostream &stream, const pos_t<T> &pos) { return stream << "(" << pos.x << ", " << pos.y << ")"; }

	pos_t<T> &operator+=(const pos_t<T> &pos) {
		x += pos.x;
		y += pos.y;
		return *this;
	}
	pos_t<T> &operator-=(const pos_t<T> &pos) {
		x -= pos.x;
		y -= pos.y;
		return *this;
	}
};


template <std::integral T> struct std::hash<pos_t<T>> {
	size_t operator()(const pos_t<T> &x) const { return hash<T>()(x.x) ^ hash<T>()(x.y); }
};

using pos = pos_t<default_pos_t>;

template <typename T>
int dijkstra(
	T start, auto is_goal, auto neighbours, auto neighbour_check = [](const T &, const T &) { return true; },
	auto cost = [](const T &, const T &) { return 1; }) {
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

		if (is_goal(current)) {
			return cost_so_far.at(current);
		}

		for (T next : neighbours(current)) {
			if (!neighbour_check(current, next))
				continue;
			int new_cost = cost_so_far[current] + cost(current, next);
			if (!cost_so_far.contains(next) || new_cost < cost_so_far[next]) {
				cost_so_far[next] = new_cost;
				chain[next] = current;
				queue.emplace(new_cost, next);
			}
		}
	}

	return -1; // no path found :(
}

template <typename T> auto dijkstra(T start, T goal, auto neighbours) {
	return dijkstra(
		start, [&](const T &c) { return c == goal; }, neighbours, [](const T &, const T &) { return true; },
		[](const T &, const T &) { return 1; });
}

template <class T> void dfs(const T &start, auto run, auto neighbours, auto neighbour_check = [](const T &, const T &) { return true; }) {
	std::vector<T> queue;
	queue.emplace_back(start);
	while (!queue.empty()) {
		T current = queue.back();
		queue.pop_back();
		if (run(current))
			continue;
		for (const T &next : neighbours(current)) {
			if (!neighbour_check(current, next))
				continue;
			queue.emplace_back(next);
		}
	}
}

namespace utils {
	constexpr bool isdigit(const char c) { return c >= '0' && c <= '9'; }
}

using utils::isdigit;

inline pos retrace_colrow(std::istream &istream) {
	const auto until = istream.tellg();
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

template <class R, class T, class M = std::identity> R sum(T values, const M &mapper = std::identity{}) {
	R sum = 0;
	for (const auto &v : values) sum += mapper(v);
	return sum;
}

template <std::unsigned_integral T> bool getbit(T number, const uint8_t pos) { return number & 1 << pos; }

// floor(logn(x) + 1) = the length of the integer in base 10
int intwidth(uint64_t x) {
	// uint64_t maximum =
	//       18446744073709551615
	// so this works :)
	/**/ if (x >= 10000000000000000000ull)
		return 20;
	else if (x >= 1000000000000000000ull)
		return 19;
	else if (x >= 100000000000000000ull)
		return 18;
	else if (x >= 10000000000000000ull)
		return 17;
	else if (x >= 1000000000000000ull)
		return 16;
	else if (x >= 100000000000000ull)
		return 15;
	else if (x >= 10000000000000ull)
		return 14;
	else if (x >= 1000000000000ull)
		return 13;
	else if (x >= 100000000000ull)
		return 12;
	else if (x >= 10000000000ull)
		return 11;
	else if (x >= 1000000000ull)
		return 10;
	else if (x >= 100000000ull)
		return 9;
	else if (x >= 10000000ull)
		return 8;
	else if (x >= 1000000ull)
		return 7;
	else if (x >= 100000ull)
		return 6;
	else if (x >= 10000ull)
		return 5;
	else if (x >= 1000ull)
		return 4;
	else if (x >= 100ull)
		return 3;
	else if (x >= 10ull)
		return 2;
	else
		return 1;
}


struct union_find {
	std::vector<size_t> parent;
	std::vector<size_t> rank;

	union_find(size_t n) {
		parent.reserve(n);
		for (size_t i = 0; i < n; i++) parent.push_back(i);
		rank = std::vector<size_t>(n, 0);
	}

	// returns true iff i and j had different parents
	bool merge(size_t i, size_t j) {
		size_t x = find(i), y = find(j);
		if (x == y)
			return false;
		size_t rx = rank[x], ry = rank[y];
		if (rx < ry) {
			parent[x] = y;
		} else {
			parent[y] = x;
			if (rx == ry)
				rank[x]++;
		}
		return true;
	}

	size_t find(size_t x) { return parent[x] == x ? x : (parent[x] = find(parent[x])); }
};

