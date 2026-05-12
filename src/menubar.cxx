#include <QMenuBar>
#include <QMenu>
#include <QtUiTools>

#include <pugixml.hpp>

#include "commands.hxx"
#include "menubar.hxx"
#include "utils.hxx"
#include "logging.hxx"

namespace easyqt {
	template<typename T>
	void addMenuItem(T* parent, pugi::xml_node itemNode) {
		QAction* item = parent->addAction(itemNode.child_value("label"));
		const char* iconName = itemNode.child_value("icon");
		if (QIcon::hasThemeIcon(iconName)) {
			item->setIcon(QIcon::fromTheme(itemNode.child_value("icon")));
		} else {
			LOG(WARN, "Warning: no such theme icon '" << iconName << "' in theme " << QIcon::themeName() << " !");
		}
		item->setShortcut(QKeySequence(itemNode.child_value("shortcut")));
		std::shared_ptr<Command> cmd = getCommand(itemNode.child_value("command"));
		if (cmd) {
			QObject::connect(
				item,
				&QAction::triggered,
				cmd.get(),
				&Command::execute
			);
		} else {
			LOG(ERROR, "No command with name '" << itemNode.child_value("command") << "' !");
		}
	}

	template<typename T>
	void addMenu(T* parent, pugi::xml_node menuNode) {
		QMenu* menu = parent->addMenu(menuNode.child_value("label"));
		if (menuNode.child("file")) {
			std::string path = getResourcePath(std::string("res:") + menuNode.child("file").value());
			pugi::xml_document xml;
			pugi::xml_parse_result result = xml.load_file(path.c_str());
			if (!result) {
				LOG(ERROR, "Failed loading menu file '" << path << "': " << result.description() << " !" << std::endl);
				return;
			}
			menuNode = xml.child("menu");
		}
		for (pugi::xml_node submenuNode: menuNode.children("menu")) {
			addMenu(menu, submenuNode);
		}
		for (pugi::xml_node itemNode: menuNode.children("menuitem")) {
			addMenuItem(menu, itemNode);
		}
	}
	
	void MenuBar::loadFromFile(std::string file) {
		std::string path = getResourcePath(file);
		pugi::xml_document xml;
		pugi::xml_parse_result result = xml.load_file(path.c_str());
		if (!result) {
			LOG(ERROR, "Failed loading menubar file '" << file << "': " << result.description() << " !");
			return;
		}
		
		pugi::xml_node menubarNode = xml.child("menubar");
		for (pugi::xml_node menuNode: menubarNode.children("menu")) {
			easyqt::addMenu(this, menuNode);
		}
		for (pugi::xml_node itemNode: menubarNode.children("menuitem")) {
			easyqt::addMenuItem(this, itemNode);
		}
	}
}

