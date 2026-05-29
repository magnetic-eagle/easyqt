#pragma once

#include <map>
#include <string>

#include "singleton.hxx"

namespace easyqt {
	class ObjectRegistry: public Singleton<ObjectRegistry> {
		public:
			template<typename T>
			static T* get(const std::string& name = "") {
				return instance()->getInternal<T>(name);
			}

			template<typename T>
			static const std::map<std::string, T*>& getAll() {
				return instance()->_storage<T>();
			}

		private:
			template<typename T>
			static std::map<std::string, T*>& _storage() {
				static std::map<std::string, T*> objects;
				return objects;
			}

			template<typename T>
			T* getInternal(const std::string& name = "") {
				auto& objects = _storage<T>();
				if (!objects.contains(name)) {
					objects[name] = new T;
					objects[name]->setName(name);
					objects[name]->init();
				}
				return objects[name];
			}
	};
}
