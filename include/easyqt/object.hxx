#pragma once

#include <string>

#include <QObject>

#include "objectregistry.hxx"

namespace easyqt {
	template<typename Base>
	class Object: public Base {
		public:
			friend ObjectRegistry;

			template<typename... Args>
			Object(Args&&... args): Base(std::forward<Args>(args)...) {};
			
			bool init() {
				if (!_inited) {
					initImpl();
					return true;
				} else {
					return false;
				}
			}
			const std::string& name() {
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
