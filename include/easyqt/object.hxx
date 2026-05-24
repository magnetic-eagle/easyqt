#pragma once

#include "objectregistry.hxx"
#include <string>

#include <QObject>

namespace easyqt {
	class Object: public QObject {
		Q_OBJECT
		public:
			friend ObjectRegistry;
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
			virtual void initImpl();
			bool setName(const std::string& name) {
				if (!_inited) {
					_name = name;
					return true;
				} else {
					return false;
				}
			}
					
		private:
			bool _inited;
			std::string _name = "";
	};
}
