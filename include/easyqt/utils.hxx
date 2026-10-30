#ifndef EASYQT_UTILS_HXX
#define EASYQT_UTILS_HXX

#include <cstdint>
#include <ctime>
#include <string>

#include <QDir>

namespace easyqt {
	std::filesystem::path getResourcePath(std::string path);
	std::string strftime(std::time_t time, const std::string& format);
	std::string strftime(std::time_t time, const char* format);

	template <typename T>
	constexpr std::string typeName() {
#if defined(__clang__)
		constexpr auto prefix = std::string_view{"[T = "};
		constexpr auto suffix = "]";
		constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
		constexpr auto prefix = std::string_view{"with T = "};
		constexpr auto suffix = "; ";
		constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
		constexpr auto prefix = std::string_view{"get_type_name<"};
		constexpr auto suffix = ">(void)";
		constexpr auto function = std::string_view{__FUNCSIG__};
#else
# error Unsupported compiler
#endif

		const auto start = function.find(prefix) + prefix.size();
		const auto end = function.find(suffix);
		const auto size = end - start;

		return std::string(function.substr(start, size));
	}

	std::string formatFileSize(std::uintmax_t size);
}

namespace std {
	template <typename F, typename T1, typename T2>
	concept IsComparator = std::invocable<F, T1, T2> && std::convertible_to<std::invoke_result_t<F, T1, T2>, bool>;

	template <typename F, typename Element, typename Key>
	concept IsProjection = std::invocable<F, Element> && std::convertible_to<std::invoke_result_t<F, Element>, Key>;
	/**
	* @brief Performs a binary search and returns an iterator to the element.
	* @param first Beginning of the range.
	* @param last End of the range.
	* @param value The value we are looking for.
	* @return Iterator to the element if found, otherwise @a last.
	*/
	template <typename ForwardIt, typename T>
	ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value) {
		auto it = std::lower_bound(first, last, value);

		if (it != last && *it == value) {
			return it;
		}
		return last;
	}
	/**
	* @brief Performs a binary search with a custom comparator and returns an iterator to the element.
	* @param first Beginning of the range.
	* @param last End of the range.
	* @param value The value we are looking for.
	* @param comp A comparison predicate (defaults to std::less).
	* @return Iterator to the element if found, otherwise @a last.
	*/
	template <typename ForwardIt, typename T, typename Compare>
	requires IsComparator<Compare, const T&, const T&>
	ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp) {
		auto it = std::lower_bound(first, last, value, comp);

		if (it != last && *it == value) {
			return it;
		}
		return last;
	}	

	/**
	* @brief Performs a binary search with a custom comparator using a projection and returns an iterator to the element.
	* @param first Beginning of the range.
	* @param last End of the range.
	* @param key The value we are looking for.
	* @param proj A projection function (e.g. [] (const User& u) { return u.id; })
	* @param comp A comparison predicate (defaults to std::less).
	* @return Iterator to the element if found, otherwise @a last.
	*/
	template <typename ForwardIt, typename Key, typename Proj, typename Compare = std::less<>>
	requires IsComparator<Compare, const Key&, const Key&> && IsProjection<Proj, typename std::iterator_traits<ForwardIt>::value_type, Key>
	ForwardIt binary_find(ForwardIt first, ForwardIt last, const Key& key, Proj proj, Compare comp = {}) {
		auto it = std::ranges::lower_bound(first, last, key, comp, proj);

		if (it != last && proj(*it) == key) {
			return it;
		}
		return last;
	}

	template <typename InputIt, typename OutputIt>
	void tolower(InputIt first, InputIt last, OutputIt d_first) {
		std::transform(first, last, d_first, [](unsigned char c) {
			return static_cast<char>(std::tolower(c));
		});
	}

	inline std::string tolower(const std::string& s) {
		std::string result;
		result.reserve(s.size());
		std::transform(s.begin(), s.end(), std::back_inserter(result), [](unsigned char c) {
			return static_cast<char>(std::tolower(c));
		});
		return result;
	}

	template <typename InputIt, typename OutputIt>
	void toupper(InputIt first, InputIt last, OutputIt d_first) {
		std::transform(first, last, d_first, [](unsigned char c) {
			return static_cast<char>(std::toupper(c));
		});
	}

	inline std::string toupper(const std::string& s) {
		std::string result;
		result.reserve(s.size());
		std::transform(s.begin(), s.end(), std::back_inserter(result), [](unsigned char c) {
			return static_cast<char>(std::toupper(c));
		});
		return result;
	}
}

#endif

