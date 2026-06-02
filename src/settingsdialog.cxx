#include <string>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>

#include <easyqt/settingsdialog.hxx>

enum class SettingType {
	Bool = 0,
	Int = 1,
	Double = 2,
	String = 3,
	Path = 4,
	PathList = 5,
};

template<typename DataType>
class Setting: public QObject {
	Q_OBJECT
	public:
		friend class SettingsPage;

		Setting(const std::string& name, const std::string& title, const DataType& defaultValue):
			_name(name), _title(title), _defaultValue(defaultValue)
		{
			_label.setText(title.c_str());
		}

		// Returns true if the setting is set to the default value
		bool isDefault() const {
			return _currentValue == _savedValue;
		};

		// Returns true if the setting has been modified by the user and has not been saved yet
		bool isModified() const {
			return _currentValue == _savedValue;
		};

		// Returns this setting's name, which is used by the settings dialog to identify this setting.
		// It should be unique within the application and should only contain letters, numbers, and hyphens to separate words.
		// Examples: username, video-quality, addon-search-paths
		const std::string& name() const { return _name; };

		// Returns this setting's title, which is displayed to the user and does not have to be unique within the application.
		// Examples: Username, Video quality, Addon search paths
		const std::string& title() const { return _title; };

		DataType currentValue() const { return _currentValue; };
		DataType defaultValue() const { return _defaultValue; };
		DataType savedValue() const { return _savedValue; };
	
	signals:
		void changed(const std::string& name);

	protected:
		virtual void createWidgets(QGridLayout& layout) = 0;
	
	private:
		std::string _name = "";
		std::string _title = "";
		DataType _defaultValue, _currentValue, _savedValue;
		QLabel _label;
};

class BoolSetting: public Setting<bool> {
	Q_OBJECT
	protected:
		void createWidgets(QGridLayout& layout) {

		}
	
};

class SettingsPage: public QScrollArea {
	public:
		SettingsPage(const std::string& name, const std::string& title): 
			_name(name), _title(title)
		{
			setLayout(&_layout);
		}

		const std::string& name() const { return _name; }
		const std::string& title() const { return _title; }

		template<typename DataType>
		void addSetting(
			const std::string& name, const std::string& title,
			SettingType type, DataType defaultValue
		);
	
	private:
		std::string _name = "";
		std::string _title = "";
		QGridLayout _layout;
};

namespace easyqt {
	void SettingsDialog::initImpl() {
		setWindowTitle("Settings");

		_layout = new QVBoxLayout();
		setLayout(_layout);

		_settingsTabs = new QTabWidget();
		_layout->addWidget(_settingsTabs);

		_buttonBox = new QDialogButtonBox(
			QDialogButtonBox::RestoreDefaults |
			QDialogButtonBox::Reset |
			QDialogButtonBox::Apply |
			QDialogButtonBox::Close
		);
		_buttonBox->button(QDialogButtonBox::RestoreDefaults)->setEnabled(false);
		_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		_buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
		_layout->addWidget(_buttonBox);
		QObject::connect(_buttonBox, &QDialogButtonBox::clicked, this, &SettingsDialog::onClicked);
	};

	void SettingsDialog::onClicked(QAbstractButton* widget) {
		QDialogButtonBox::StandardButton role = _buttonBox->standardButton(widget);
		if (role == QDialogButtonBox::Close) {
			close();
		}
	}

	void SettingsDialog::loadFromFile(std::filesystem::path path) {
	}
}
