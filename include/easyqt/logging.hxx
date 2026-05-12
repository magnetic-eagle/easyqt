#ifndef EASYQT_LOGGING_HXX
#define EASYQT_LOGGING_HXX

#include <ctime>
#include <iostream>
#include <string>
#include <sstream>

#include "singleton.hxx"
#include "utils.hxx"

namespace easyqt {
	class Logger: public Singleton<Logger> {
		public:
			enum class LogLevel: short {
				DEBUG = 0,
				INFO = 1,
				WARN = 2,
				WARNING = 2,
				ALERT = 3,
				ERROR = 4,
				FATAL = 5,
				UNKNOWN = 6,
			};
			
			Logger() {};
			
			template<typename T>
			void log(LogLevel level, const T& msg) {
				if (level >= _level) {
					std::ostringstream sstream;
					sstream << "[" << level << "] " << strftime(std::time(nullptr), "%T") << " " << msg << std::endl;
					if (level >= LogLevel::ERROR) {
						std::cerr << sstream.str();
						std::cerr.flush();
					} else {
						std::cout << sstream.str();
						std::cout.flush();
					}
				}
			}
			friend std::ostream& operator<<(std::ostream&, Logger::LogLevel);
			
			static const LogLevel getLogLevelForString(std::string levelName);
			static const LogLevel getLogLevelForString(char* levelName);
			
			LogLevel logLevel() {
				return _level;
			}
			void setLogLevel(LogLevel level) {
				_level = level;
			}
			void setLogLevel(std::string levelName) {
				_level = getLogLevelForString(levelName);
			}
			void setLogLevel(char* levelName) {
				_level = getLogLevelForString(levelName);
			}
		
		private:
			LogLevel _level {LogLevel::WARNING};
	};
	std::ostream& operator<<(std::ostream& s, const easyqt::Logger::LogLevel level);
}

std::ostream& operator<<(std::ostream& s, QString value);

namespace std {
	string to_string(easyqt::Logger::LogLevel level);
}

#define LOG(level, expr) \
	{ \
		std::stringstream ss; \
		ss << expr; \
		std::string str = ss.str(); \
		easyqt::Logger::instance()->log(easyqt::Logger::LogLevel::level, ss.str()); \
	}

#endif

