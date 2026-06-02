#pragma once

#include <string>

#include "objectregistry.hxx"

namespace easyqt {
	class __DefaultBase {
		public:
			__DefaultBase() = default;
	};

	template<typename Base = __DefaultBase>
	class Object: public Base {
		public:
			friend class ObjectRegistry;

			template<typename... Args>
			Object(Args&&... args): Base(std::forward<Args>(args)...) {};
			
			bool init() {
				if (!_inited) {
					initImpl();
					_inited = true;
					return true;
				} else {
					return false;
				}
			}
			const std::string& name() const {
				return _name;
			}
		
		protected:
			virtual void initImpl() {};
			bool setName(const std::string& name) {
				if (!_inited) {
					_name = name;
					return true;
				} else {
					return false;
				}
			}
					
		private:
			bool _inited = false;
			std::string _name = "";
	};
}
