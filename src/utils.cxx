#include <cstdint>
#include <ctime>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include <QFile>

#include "utils.hxx"

namespace easyqt {
	std::filesystem::path getResourcePath(std::string path) {
		QFile resolver(path.c_str());
		return resolver.fileName().toStdString();
	}
	
	std::string strftime(std::time_t time, const std::string& format) {
		std::tm* local_time = std::localtime(&time);
		char output[256];
		std::strftime(output, sizeof(output), format.c_str(), local_time);
		return output;
	}
	std::string strftime(std::time_t time, const char* format) {
		std::tm* local_time = std::localtime(&time);
		char output[256];
		std::strftime(output, sizeof(output), format, local_time);
		return output;
	}

	std::string formatFileSize(std::uintmax_t size) {
		static std::vector<std::string> units = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB"};
		int unit_index = 0;
		double convertedSize = size;
		while (convertedSize >= 1000 && unit_index < units.size() - 1) {
			convertedSize /= 1000;
			unit_index++;
		}
		std::stringstream ss;
		ss << std::fixed << std::setprecision(1) << convertedSize << " " << units[unit_index];
		return ss.str();
	}
}

namespace std {
	namespace filesystem {
		std::filesystem::path longest_common_basepath(const std::vector<std::filesystem::path>& paths) {
			if (paths.empty()) {
				return "";
			}

			// Store the common path components as a vector of strings.
			std::vector<std::string> common;

			// Initialize common with components of the first path
			for (const auto& part : paths[0]) {
				common.push_back(part.string());
			}

			// Iteratively narrow down the common components
			for (const auto& path: paths) {
				std::vector<std::string> current_parts;
				for (const auto& part : path) {
					current_parts.push_back(part.string());
				}

				std::vector<std::string> next_common;
				size_t min_size = std::min(common.size(), current_parts.size());
				
				for (size_t j = 0; j < min_size; ++j) {
					if (common[j] == current_parts[j]) {
						next_common.push_back(common[j]);
					} else {
						break;
					}
				}
				common = std::move(next_common);
			}

			std::filesystem::path result;
			for (const auto& s : common) {
				if (result.empty()) {
					result = s;
				} else {
					result /= s;
				}
			}

			// Handle the edge case where paths are absolute but have no common components 
			// beyond the root (e.g., "/a" and "/b").
			if (common.empty() && paths[0].is_absolute()) {
				return paths[0].root_path();
			}

			return result;
		}
	}
}


