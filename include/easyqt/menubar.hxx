#ifndef EASYQT_MENUBAR_HXX
#define EASYQT_MENUBAR_HXX

#include <qmenubar.h>
#include <string>

#include <QMenuBar>

#include "object.hxx"

namespace easyqt {
	class MenuBar: public Object<QMenuBar> {
		Q_OBJECT
		public:
			MenuBar(const std::string& name = ""){};
			void loadFromFile(std::string file);

			void initImpl() {};
	};
}
#endif
