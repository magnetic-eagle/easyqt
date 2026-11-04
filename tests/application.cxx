#include <QMainWindow>
#include <QTimer>

#include <easyqt/application.hxx>

int main(int argc, char *argv[]) {
	easyqt::Application app(argc, argv);
	QTimer::singleShot(1000, &app, &QApplication::quit);
	return app.exec();
}

