#pragma once
#include <variant>

namespace utils {
	template <class T, class... Ts> struct index_in_pack;

	template <class T, class... Ts> struct index_in_pack<T, T, Ts...> : std::integral_constant<size_t, 0> {};

	template <class T, class U, class... Ts>
	struct index_in_pack<T, U, Ts...> : std::integral_constant<size_t, 1 + index_in_pack<T, Ts...>::value> {};

	template <class... Ts> struct overloaded : Ts... {
		using Ts::operator()...;
	};

	template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

	template <class... Ts> struct variant : std::variant<Ts...> {
		using _base = std::variant<Ts...>;
		using _base::_base; // inherit constructors

		static consteval size_t size() { return sizeof...(Ts); }

		template <class T> static consteval size_t index_of() { return index_in_pack<T, Ts...>::value; };
		template <class T> constexpr T get() { return std::get<T>(*this); }

		template <class T> constexpr bool is() { return std::holds_alternative<T>(*this); }

		constexpr bool is(size_t index) { return this->index() == index; }

		template <class... Fs> constexpr auto visit(Fs... fs) { return std::visit(overloaded{fs...}, *this); }

		template <size_t start = 0, size_t end = size(), bool inclusive> static void for_each(auto fn) {
			(
				[&] {
					using T = Ts;
					constexpr auto index = index_of<T>();
					if constexpr (index >= start && (inclusive ? (index <= end) : (index < end)))
						fn.template operator()<T>(index_of<T>());
				}(),
				...);
		}

		template <size_t start = 0, size_t end = size()> static void for_each(auto fn) { for_each<start, end, false>(fn); }

		template <class Start, class EndInclusive> static void for_each(auto fn) {
			for_each<index_of<Start>(), index_of<EndInclusive>(), true>(fn);
		}

		static void for_each(auto fn) { (fn.template operator()<Ts>(), ...); }
	};
} // namespace utils
