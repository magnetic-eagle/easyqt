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
		ss << std::fixed << std::setprecision(2) << convertedSize << " " << units[unit_index];
		return ss.str();
	}
}


