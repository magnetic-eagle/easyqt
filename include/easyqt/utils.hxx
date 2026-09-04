#ifndef EASYQT_UTILS_HXX
#define EASYQT_UTILS_HXX

#include <ctime>
#include <string>

#include <QDir>

namespace easyqt {
	std::string getResourcePath(std::string path);
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
}

namespace std {
	string tolower(const string& s);
	string toupper(const string& s);
}

#endif

