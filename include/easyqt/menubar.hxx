#ifndef EASYQT_MENUBAR_HXX
#define EASYQT_MENUBAR_HXX

#include <string>

#include <QMenuBar>
#include "singleton.hxx"

namespace easyqt {
	class MenuBar: public Singleton<MenuBar, QMenuBar> {
		Q_OBJECT
		public:
			MenuBar(std::string name = "") {};
			void loadFromFile(std::string file);
	};
}
#endif
