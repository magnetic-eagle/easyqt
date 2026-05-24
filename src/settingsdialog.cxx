#include <easyqt/settingsdialog.hxx>
#include <qboxlayout.h>
#include <qdialogbuttonbox.h>
#include <qlayout.h>
#include <qtabwidget.h>

namespace easyqt {
	void SettingsDialog::initImpl() {
		setWindowTitle("Settings");

		_layout = new QVBoxLayout();
		setLayout(_layout);

		_settingsTabs = new QTabWidget();
		_layout->addWidget(_settingsTabs);

		_buttonBox = new QDialogButtonBox(
			QDialogButtonBox::RestoreDefaults |
			QDialogButtonBox::Ok |
			QDialogButtonBox::Apply |
			QDialogButtonBox::Cancel
		);
		_layout->addWidget(_buttonBox);
	};

	void SettingsDialog::loadFromFile(std::filesystem::path path) {
	}
}
