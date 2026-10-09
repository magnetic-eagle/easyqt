#pragma once

#include <filesystem>

#include <QLineEdit>
#include <QToolButton>
#include <QWidget>

namespace easyqt {
	class PathEntry: public QWidget {
		Q_OBJECT
		public:
			enum class PathType {
				File,
				Directory,
			};

			PathEntry(QWidget* parent = nullptr);
			
			void setPath(std::filesystem::path path);
			void setPathType(PathType type);

			std::filesystem::path path() const { return _path; };
		
		signals:
			void pathChanged(std::filesystem::path path);
		
		public slots:
			void onButtonClicked();
			void onEntryTextChanged(const QString& text);
		
		private:
			std::filesystem::path _path;
			PathType _type;
			QLineEdit* _entry;
			QToolButton* _button;
	};
}
