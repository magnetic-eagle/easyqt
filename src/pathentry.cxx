#include <filesystem>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QWidget>

# include "pathentry.hxx"

namespace easyqt {
	PathEntry::PathEntry(QWidget* parent): QWidget(parent) {
		auto layout = new QHBoxLayout(this);
		setLayout(layout);

		_entry = new QLineEdit(this);
		QObject::connect(_entry, &QLineEdit::textChanged, this, &PathEntry::onEntryTextChanged);
		layout->addWidget(_entry);

		_button = new QToolButton(this);
		QObject::connect(_button, &QToolButton::clicked, this, &PathEntry::onButtonClicked);
		_button->setIcon(QIcon::fromTheme("document-open"));
		layout->addWidget(_button);
	}

	void PathEntry::setPath(std::filesystem::path path) {
		const QSignalBlocker blocker(_entry);
		_path = path;
		_entry->setText(path.c_str());
		emit pathChanged(_path);
	}

	void PathEntry::setPathType(PathType type) {
		_type = type;
	}

	void PathEntry::onEntryTextChanged(const QString& text) {
		_path = text.toStdString();
		emit pathChanged(_path);
	}

	void PathEntry::onButtonClicked() {
		QFileDialog dialog(this);
		if (_type == PathType::File) {
			dialog.setFileMode(QFileDialog::AnyFile);
			dialog.setWindowTitle("Choose file");
		} else if (_type == PathType::Directory) {
			dialog.setFileMode(QFileDialog::Directory);
			dialog.setWindowTitle("Choose directory");
		}

		if (dialog.exec() == QDialog::Accepted) {
			const QString path = dialog.selectedFiles().first();
			if (!path.isEmpty()) {
				setPath(path.toStdString());
			}
		}
	}
}