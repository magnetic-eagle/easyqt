#include <QFile>

#include "menubar.hxx"
#include "objectregistry.hxx"
#include "pathentry.hxx"
#include "scrollarea.hxx"
#include "spinner.hxx"
#include "uiloader.hxx"

namespace easyqt {
	QWidget* UiLoader::createWidget(const QString& widgetName, QWidget* parentWidget, const QString& name) {
		QWidget* w;
		if (widgetName == "easyqt::MenuBar") {
			w = ObjectRegistry::instance()->get<MenuBar>(name.toStdString());
		} else if (widgetName == "easyqt::PathEntry") {
			w = new PathEntry;
		} else if (widgetName == "easyqt::ScrollArea") {
			w = new ScrollArea;
		} else if (widgetName == "easyqt::Spinner") {
			w = new Spinner;
		} else {
			return QUiLoader::createWidget(widgetName, parentWidget, name);
		}
		w->setObjectName(name);
		w->setParent(parentWidget);
		return w;
	}
}

