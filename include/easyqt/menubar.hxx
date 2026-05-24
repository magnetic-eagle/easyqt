#ifndef EASYQT_MENUBAR_HXX
#define EASYQT_MENUBAR_HXX

#include <string>

#include <QMenuBar>

#include "object.hxx"

namespace easyqt {
	class MenuBar: public QMenuBar, public Object {
		Q_OBJECT
		public:
			MenuBar(const std::string& name = ""){};
			void loadFromFile(std::string file);
	};
}
#endif
