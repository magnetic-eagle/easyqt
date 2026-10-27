#pragma once

#include <map>
#include <memory>
#include <string>

#include "singleton.hxx"

namespace easyqt {
	class ObjectRegistry: public Singleton<ObjectRegistry> {
		friend class Singleton<ObjectRegistry>;

		public:
			template<typename T>
			static T* get(const std::string& name = "") {
				return instance()->getInternal<T>(name);
			}

			template<typename T>
			static const std::map<std::string, T*>& getAll() {
				return instance()->_storage<T>();
			}

			static void cleanup() {
				instance()->_storages.clear();
			}

		private:
			struct StorageBase {
				virtual ~StorageBase() = default;
			};

			template<typename T>
			struct Storage: StorageBase {
				~Storage() override {
					for (auto& [name, object] : objects)
					delete object;
				}

				std::map<std::string, T*> objects;
			};

			std::map<std::string, std::unique_ptr<StorageBase>> _storages;

			template<typename T>
			Storage<T>& _storage() {
				const auto key = typeid(T).name();

				auto& storage = _storages[key];
				if (!storage) {
					storage = std::make_unique<Storage<T>>();
				}

				return *static_cast<Storage<T>*>(storage.get());
			};

			template<typename T>
			T* getInternal(const std::string& name = "") {
				auto& objects = _storage<T>().objects;
				if (!objects.contains(name)) {
					auto* object = new T;
					objects[name] = object;
					object->setName(name);
					object->init();
				}

				return objects[name];
			}
	};
}
