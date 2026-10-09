#include <exception>
#include <filesystem>
#include <iomanip>
#include <string>
#include <vector>

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStandardPaths>

#include <pugixml.hpp>

#include "logging.hxx"
#include "pathentry.hxx"
#include "settingsdialog.hxx"
#include "utils.hxx"

static std::map<std::string, SettingType> typeMapping = {
	{"bool", SettingType::Bool},
	{"int", SettingType::Int},
	{"double", SettingType::Double},
	{"string", SettingType::String},
	{"path", SettingType::Path},
};

static std::map<std::string, SettingLevel> levelMapping = {
	{"basic", SettingLevel::Basic},
	{"advanced", SettingLevel::Advanced},
	{"developer", SettingLevel::Developer},
};

template<typename T>
struct SettingTraits;

template<>
struct SettingTraits<bool> {
	using Widget = QCheckBox;
	struct Config {
		SettingType type = SettingType::Bool;
		const std::string typeName = "bool";
	};
};

template<>
struct SettingTraits<int> {
	using Widget = QSpinBox;
	struct Config {
		SettingType type = SettingType::Int;
		const std::string typeName = "int";
		int min = 0;
		int max = 100;
		int step = 1;
	};
};

template<>
struct SettingTraits<double> {
	using Widget = QDoubleSpinBox;
	struct Config {
		SettingType type = SettingType::Double;
		const std::string typeName = "double";
		double min = 0;
		double max = 100;
		double step = 0.1;
	};
};

template<>
struct SettingTraits<std::string> {
	using Widget = QLineEdit;
	struct Config {
		SettingType type = SettingType::String;
		const std::string typeName = "std::string";
	};
};

template<>
struct SettingTraits<std::filesystem::path> {
	using Widget = easyqt::PathEntry;
	struct Config {
		SettingType type = SettingType::Path;
		const std::string typeName = "std::filesystem::path";
		easyqt::PathEntry::PathType pathType = easyqt::PathEntry::PathType::File;
	};
};

bool parseValue(pugi::xml_node valueNode, const std::string& settingName, bool defaultValue) {
	if (valueNode) {
		const std::string& valueString = valueNode.text().get();
		if (valueString == "true") {
			defaultValue = true;
		} else if (valueString == "false") {
			defaultValue = false;
		} else {
			LOG(ERROR, "Invalid boolean value " << std::quoted(valueString) << " for setting " << std::quoted(settingName));
		}
	}
	return defaultValue;
}

int parseValue(pugi::xml_node valueNode, const std::string& settingName, int defaultValue) {
	if (valueNode) {
		const std::string& valueString = valueNode.text().get();
		try {
			defaultValue = std::stoi(valueString);
		} catch (const std::exception& e) {
			LOG(ERROR, "Invalid integer value " << std::quoted(valueString) << " for setting " << std::quoted(settingName));
		}
	}
	return defaultValue;
}

double parseValue(pugi::xml_node valueNode, const std::string& settingName, double defaultValue) {
	if (valueNode) {
		const std::string& valueString = valueNode.text().get();
		try {
			defaultValue = std::stod(valueString);
		} catch (const std::exception& e) {
			LOG(ERROR, "Invalid double value " << std::quoted(valueString) << " for setting " << std::quoted(settingName));
		}
	}
	return defaultValue;
}

template<typename DataType>
class Setting: public SettingBase {
	public:
		using Traits = SettingTraits<DataType>;
		friend class SettingsDialog;
		friend class SettingsPage;

		Setting(const std::string& name, const std::string& label, SettingLevel level, pugi::xml_node node):
			SettingBase(name, label, level, node)
		{
		}

		// Returns true if the setting is set to the default value
		virtual bool isDefault() const override{
			return _currentValue == _defaultValue;
		}

		//s Returns true if the setting has been modified by the user and has not been saved yet
		virtual bool isModified() const override {
			return _currentValue != _savedValue;
		}

		// Returns true if the setting has been saved.
		virtual bool isSaved() const override {
			return _currentValue == _savedValue;
		}

		DataType currentValue() const { return _currentValue; }
		DataType defaultValue() const { return _defaultValue; }
		DataType savedValue() const { return _savedValue; }
		virtual std::string savedValueString() const override;

		const SettingType type() const override { return _config.type; }
		const std::string& typeName() const override { return _config.typeName; }

	public slots:
		void widgetValueChanged(DataType value) {
			_currentValue = value;

			emit changed(_name);
		};

	protected:
		virtual Traits::Widget* createWidget() override {
			LOG(WARNING, "Cannot create setting widget for type " << std::quoted(easyqt::typeName<DataType>()) << ": not implemented");
			return nullptr;
		};

		virtual void parseValues(pugi::xml_node userValueNode) override {
			LOG(WARNING, "Cannot parse config values for setting of type " << std::quoted(easyqt::typeName<DataType>()) << ": not implemented");
		}

		virtual void applyValue() override {
			LOG(WARNING, "Cannot apply new value to setting widget for type " << std::quoted(easyqt::typeName<DataType>()) << ": not implemented");
		}

		void saveValue() override { _savedValue = _currentValue; }

		void setSavedValue(DataType value) {
			_currentValue = _savedValue = value;
		}
		void resetToSavedValue() override {
			_currentValue = _savedValue;
			applyValue();
		}
		void resetToDefaultValue() override {
			_currentValue = _defaultValue;
			applyValue();
		}
	
	private:
		DataType _defaultValue, _currentValue, _savedValue;
		Traits::Widget* _widget;
		Traits::Config _config;
};

template<>
QCheckBox* Setting<bool>::createWidget() {
	auto _widget = new QCheckBox();
	_widget->setChecked(_savedValue);
	connect(_widget, &QCheckBox::toggled, this, &Setting::widgetValueChanged);

	return _widget;
}

template<>
void Setting<bool>::parseValues(pugi::xml_node userValueNode) {
	_defaultValue = parseValue(_node.child("default"), _name, _defaultValue);
	_currentValue = _savedValue = parseValue(userValueNode, _name, _defaultValue);
}

template<>
void Setting<bool>::applyValue() {
	_widget->setChecked(_currentValue);
}

template<>
std::string Setting<bool>::savedValueString() const {
	return _savedValue ? "true" : "false";
}

template<>
QSpinBox* Setting<int>::createWidget() {
	_widget = new QSpinBox();
	_widget->setValue(_savedValue);
	_widget->setRange(_config.min, _config.max);
	_widget->setSingleStep(_config.step);
	connect(_widget, &QSpinBox::valueChanged, this, &Setting::widgetValueChanged);

	return _widget;
}

template<>
void Setting<int>::parseValues(pugi::xml_node userValueNode) {
	_defaultValue = _currentValue = _savedValue = parseValue(_node.child("default"), _name, _defaultValue);
	_currentValue = _savedValue = parseValue(userValueNode, _name, _defaultValue);
	_config.min = parseValue(_node.child("min"), _name, _config.min);
	_config.max = parseValue(_node.child("max"), _name, _config.max);
	_config.step = parseValue(_node.child("step"), _name, _config.step);
}

template<>
std::string Setting<int>::savedValueString() const {
	return std::to_string(_savedValue);
}

template<>
void Setting<int>::applyValue() {
	_widget->setValue(_currentValue);
}

template<>
QDoubleSpinBox* Setting<double>::createWidget() {
	_widget = new QDoubleSpinBox();
	_widget->setValue(_savedValue);
	_widget->setRange(_config.min, _config.max);
	_widget->setSingleStep(_config.step);
	connect(_widget, &QDoubleSpinBox::valueChanged, this, &Setting::widgetValueChanged);

	return _widget;
}

template<>
void Setting<double>::parseValues(pugi::xml_node userValueNode) {
	_defaultValue = _currentValue = _savedValue = parseValue(_node.child("default"), _name, _defaultValue);
	_currentValue = _savedValue = parseValue(userValueNode, _name, _defaultValue);
	_config.min = parseValue(_node.child("min"), _name, _config.min);
	_config.max = parseValue(_node.child("max"), _name, _config.max);
	_config.step = parseValue(_node.child("step"), _name, _config.step);
}

template<>
std::string Setting<double>::savedValueString() const {
	return std::to_string(_savedValue);
}

template<>
void Setting<double>::applyValue() {
	_widget->setValue(_currentValue);
}

template<>
QLineEdit* Setting<std::string>::createWidget() {
	_widget = new QLineEdit();
	_widget->setText(_savedValue.c_str());
	connect(_widget, &QLineEdit::textChanged, [this](const QString& text) {
		this->widgetValueChanged(text.toStdString());
	});

	return _widget;
}

template<>
void Setting<std::string>::parseValues(pugi::xml_node userValueNode) {
	_defaultValue = _currentValue = _savedValue = _node.child("default").text().get();
	if (userValueNode) {
		_currentValue = _savedValue = userValueNode.text().get();
	}
}

template<>
std::string Setting<std::string>::savedValueString() const {
	return _savedValue;
}

template<>
void Setting<std::string>::applyValue() {
	_widget->setText(_currentValue.c_str());
}

template<>
easyqt::PathEntry* Setting<std::filesystem::path>::createWidget() {
	_widget = new easyqt::PathEntry();
	_widget->setPath(_savedValue);
	_widget->setPathType(_config.pathType);
	connect(_widget, &easyqt::PathEntry::pathChanged, this, &Setting::widgetValueChanged);

	return _widget;
}

template<>
void Setting<std::filesystem::path>::parseValues(pugi::xml_node userValueNode) {
	_defaultValue = _currentValue = _savedValue = _node.child("default").text().get();
	if (userValueNode) {
		_currentValue = _savedValue = userValueNode.text().get();
	}
	const std::string& pathTypeString = _node.child("path-type").text().get();
	if (pathTypeString == "file") {
		_config.pathType = easyqt::PathEntry::PathType::File;
	} else if (pathTypeString == "directory") {
		_config.pathType  = easyqt::PathEntry::PathType::Directory;
	} else {
		LOG(WARN, "Invalid or unspecified path type for setting " << std::quoted(_name));
	}
}

template<>
std::string Setting<std::filesystem::path>::savedValueString() const {
	return _savedValue.string();
}

template<>
void Setting<std::filesystem::path>::applyValue() {
	_widget->setPath(_currentValue);
}

class SettingsPage: public QScrollArea {
	public:
		SettingsPage(const std::string& name, const std::string& label): 
			_name(name), _label(label)
		{
			setLayout(&_layout);
			// this might make things look weird if someone uses 9999 settings on one page
			_layout.setRowStretch(9999, 1);
			_layout.setColumnStretch(1, 1);
			_layout.setSpacing(10);
		}

		const std::string& name() const { return _name; }
		const std::string& label() const { return _label; }


		SettingBase* addSetting(
			const std::string& name, const std::string& label,
			SettingType type, SettingLevel level, pugi::xml_node configNode,
			pugi::xml_node userValueNode
		) {
			SettingBase* setting;
			switch (type) {
				case SettingType::Bool:
					setting = new Setting<bool>(name, label, level, configNode);
					break;

				case SettingType::Int:
					setting = new Setting<int>(name, label, level, configNode);
					break;

				case SettingType::Double:
					setting = new Setting<double>(name, label, level, configNode);
					break;

				case SettingType::String:
					setting = new Setting<std::string>(name, label, level, configNode);
					break;

				case SettingType::Path:
					setting = new Setting<std::filesystem::path>(name, label, level, configNode);
					break;
			}

			setting->parseValues(userValueNode);
			_settings.push_back(setting);
			_lastRow += 1;
			_layout.addWidget(new QLabel(label.c_str()), _lastRow, 0);
			_layout.addWidget(setting->createWidget(), _lastRow, 1);

			return setting;
		}
	
	private:
		std::string _name = "";
		std::string _label = "";
		QGridLayout _layout;
		int _lastRow = -1;
		std::vector<SettingBase*> _settings;
};

namespace easyqt {
	void SettingsDialog::initImpl() {
		setWindowTitle("Settings");
		setModal(true);

		_layout = new QVBoxLayout();
		setLayout(_layout);

		_settingsLayout = new QHBoxLayout();
		_layout->addLayout(_settingsLayout);

		_pageList = new QListWidget();
		_pageList->setStyleSheet("QListWidget::item { padding: 6px 12px; }");
		_pageList->setSelectionMode(QAbstractItemView::SingleSelection);
		_settingsLayout->addWidget(_pageList);
		QObject::connect(_pageList, &QListWidget::currentItemChanged, this, &SettingsDialog::onPageSelected);

		_pageContainer = new QStackedWidget();
		_settingsLayout->addWidget(_pageContainer, 1);

		_buttonBox = new QDialogButtonBox(
			QDialogButtonBox::RestoreDefaults |
			QDialogButtonBox::Reset |
			QDialogButtonBox::Apply |
			QDialogButtonBox::Close |
			QDialogButtonBox::Ok
		);
		_layout->addWidget(_buttonBox);
		QObject::connect(_buttonBox, &QDialogButtonBox::clicked, this, &SettingsDialog::onClicked);

		_path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toStdString();
		_path /= "settings.xml";
		std::filesystem::create_directory(_path.parent_path());
	}

	void SettingsDialog::onPageSelected(QListWidgetItem* current, QListWidgetItem* previous) {
		std::string selectedName = current->data(Qt::UserRole).toString().toStdString();
		for (SettingsPage* page: _pages) {
			if (page->name() == selectedName) {
				_pageContainer->setCurrentWidget(page);
				return;
			}
		}
		LOG(ERROR, "Could not find settings page with selected name " << std::quoted(selectedName));
	}

	void SettingsDialog::onClicked(QAbstractButton* widget) {
		QDialogButtonBox::StandardButton role = _buttonBox->standardButton(widget);
		if (role == QDialogButtonBox::Ok) {
			applySettings();
			writeSettingsToFile();
			close();
		} else if (role == QDialogButtonBox::Apply) {
			applySettings();
			writeSettingsToFile();
		} else if (role == QDialogButtonBox::Reset) {
			resetSettingsToSaved();
		} else if (role == QDialogButtonBox::RestoreDefaults) {
			resetSettingsToDefault();
		}
		if (role == QDialogButtonBox::Close) {
			if (_numUnsavedSettings > 0) {
				QMessageBox::StandardButton button = QMessageBox::question(
					this,
					"Discard setting changes?",
					"Close the settings dialog without saving your changes ?",
					QMessageBox::Yes | QMessageBox::No,
					QMessageBox::No
				);
				if (button == QMessageBox::Yes) {
					resetSettingsToSaved();
				} else {
					return;
				}
			}
			close();
		}
		updateButtonStates();
	}

	void SettingsDialog::onSettingChanged(const std::string& settingName) {
		updateButtonStates();
	}

	void SettingsDialog::applySettings() {
		for (auto& setting: _settings) {
			setting.second->saveValue();
		}
	}

	void SettingsDialog::resetSettingsToDefault() {
		for (auto& setting: _settings) {
			setting.second->resetToDefaultValue();
		}
	}

	void SettingsDialog::resetSettingsToSaved() {
		for (auto& setting: _settings) {
			setting.second->resetToSavedValue();
		}
	}

	void SettingsDialog::updateButtonStates() {
		_numNonDefaultSettings = 0;
		_numUnsavedSettings = 0;
		for (auto& pair: _settings) {
			auto& s = pair.second;
			if (!s->isDefault()) {
				_numNonDefaultSettings++;
			}
			if (!s->isSaved()) {
				_numUnsavedSettings++;
			}
		}
		if (_numUnsavedSettings > 0) {
			_buttonBox->button(QDialogButtonBox::Reset)->setEnabled(true);
			_buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
			_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
		} else {
			_buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
			_buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
			_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		}
		if (_numNonDefaultSettings > 0) {
			_buttonBox->button(QDialogButtonBox::RestoreDefaults)->setEnabled(true);
		} else {
			_buttonBox->button(QDialogButtonBox::RestoreDefaults)->setEnabled(false);
		}
	}

	void SettingsDialog::writeSettingsToFile() {
		pugi::xml_document document;

		pugi::xml_node settingsNode = document.append_child("settings");

		for (const auto& [name, setting]: _settings) {
			pugi::xml_node settingNode = settingsNode.append_child("setting");
			settingNode.append_child("name").text().set(name.c_str());
			settingNode.append_child("value").text().set(setting->savedValueString().c_str());
		}

		if (!document.save_file(_path.c_str())) {
			LOG(ERROR, "Failed saving user settings");
		} else {
			LOG(DEBUG, "User settings saved to " << std::quoted(_path.string()));
		}
	}
	
	void SettingsDialog::rebuild() {
		_pages.clear();
		_settings.clear();
		pugi::xml_parse_result result = _settingsStorage.load_file(_factoryPath.c_str());
		if (!result) {
			LOG(ERROR, "Failed loading settings definition file " << std::quoted(_factoryPath.string()) << ": " << result.description() << " !");
			return;
		}
		pugi::xml_document userSettingsDocument;
		result = userSettingsDocument.load_file(_path.c_str());
		if (!result) {
			LOG(ERROR, "Failed loading user settings file " << std::quoted(_path.string()) << ": " << result.description() << " !");
		}

		std::map<std::string, pugi::xml_node> userSettings;
		for (pugi::xml_node settingNode: userSettingsDocument.child("settings").children("setting")) {
			const std::string& settingName = settingNode.child_value("name");
			if (settingName.empty()) {
				LOG(WARN, "Skipping user setting with empty or missing name");
			}
			userSettings[settingName] = settingNode.child("value");
		}
		
		pugi::xml_node settings = _settingsStorage.child("settings");
		for (pugi::xml_node categoryNode: settings.children("category")) {
			std::string name = categoryNode.child_value("name");
			std::string label = categoryNode.child_value("label");

			if (name.empty()) {
				LOG(ERROR, "Cannot add settings page without name");
				continue;
			}

			if (label.empty()) {
				LOG(WARN, "Adding settings page with empty label, using name instead");
				label = name;
			} else {
				LOG(DEBUG, "Adding settings page with label " << std::quoted(label));
			}
			QListWidgetItem* item = new QListWidgetItem(label.c_str());
			item->setData(Qt::UserRole, name.c_str());
			_pageList->addItem(item);
			SettingsPage* page = new SettingsPage(name, label);
			_pages.push_back(page);
			_pageContainer->addWidget(page);

			for (pugi::xml_node settingNode: categoryNode.children("setting")) {
				std::string settingName = settingNode.child_value("name");
				std::string settingLabel = settingNode.child_value("label");
				std::string levelString = settingNode.child_value("level");

				pugi::xml_node configNode;
				bool multipleconfigNodesWarningLogged = false;
				std::string unknownNode;

				for (pugi::xml_node child: settingNode.children()) {
					const std::string name = child.name();

					if (name == "name") {
						settingName = child.text().as_string();
						continue;
					} else if (name == "label") {
						settingLabel = child.text().as_string();
						continue;
					} else if (name == "level") {
						levelString = child.text().as_string();
					} else if (typeMapping.contains(name)) {
						if (configNode) {
							if (multipleconfigNodesWarningLogged) {
								LOG(WARN, "Setting " << std::quoted(settingName) << " has multiple type nodes");
								multipleconfigNodesWarningLogged = true;
							}
							continue;
						}

						configNode = child;
					} else if (unknownNode.empty()) {
						unknownNode = name;
					}
				}

				if (!configNode) {
					if (!unknownNode.empty()) {
						LOG(ERROR, "Cannot add setting " << std::quoted(settingName) << " with unknown type " << std::quoted(unknownNode));
					} else {
						LOG(ERROR, "Cannot add setting " << std::quoted(settingName) << " with no type specified");
					}
					continue;
				}
				const std::string typeString = configNode.name();
				SettingType type;
				try {
					type = typeMapping.at(typeString);
				} catch (const std::out_of_range&) {
					LOG(ERROR, "Cannot add setting with unknown type " << std::quoted(typeString));
					continue;
				}
			
				if (settingName.empty()) {
					LOG(ERROR, "Cannot add setting without name");
					continue;
				} else if (_settings.contains(settingName)) {
					LOG(ERROR, "Setting with name " << std::quoted(settingName) << " already present, cannot add duplicate name");
				}

				if (settingLabel.empty()) {
					LOG(WARN, "Adding setting with empty label, using name " << std::quoted(settingName) << " instead");
					settingLabel = settingName;
				} else {
					LOG(DEBUG, "Adding setting with name " << std::quoted(settingName) << " label " << std::quoted(settingLabel));
				}

				SettingLevel level;
				try {
					level = levelMapping.at(levelString);
				} catch (std::out_of_range& e) {
					LOG(WARN, "Unknown setting level " << std::quoted(levelString) << "; falling back to " << std::quoted("basic"));
					level = SettingLevel::Basic;
				}

				pugi::xml_node defaultValueNode = configNode.child("default");
				bool hasDefaultValue = defaultValueNode;
				if (!hasDefaultValue) {
					LOG(ERROR, "Cannot add setting " << std::quoted(settingName) << " without default value");
					continue;
				}

				pugi::xml_node userValueNode;
				if (userSettings.contains(settingName)) {
					userValueNode = userSettings[settingName];
				}
				_settings[settingName] = page->addSetting(settingName, settingLabel, type, level, configNode, userValueNode);
				QObject::connect(_settings[settingName], &SettingBase::changed, this, &SettingsDialog::onSettingChanged);
			}
		}
		
		if (_pageList->count() > 0) {
		    _pageList->setCurrentRow(0);
		}

		const int width = _pageList->sizeHintForColumn(0) + 2 * _pageList->frameWidth();
		_pageList->setFixedWidth(width);
		updateButtonStates();
	}

	template<typename DataType>
	DataType SettingsDialog::settingValue(const std::string& settingName) const {
		const auto settingIt = _settings.find(settingName);
		if (settingIt == _settings.end()) {
			throw std::out_of_range("No setting with name \"" + settingName + "\" found");
		}
		const auto* setting = dynamic_cast<const Setting<DataType>*>(settingIt->second);

		if (!setting) {
			throw std::invalid_argument(
				"Cannot get value as \"" + easyqt::typeName<DataType>() + " from setting \"" +
				settingName + "\" with type \"" + settingIt->second->typeName() + "\""
			);
		}

		return setting->savedValue();
	}

	template<typename DataType>
	void SettingsDialog::setSettingValue(const std::string& settingName, const DataType& value) {
		const auto settingIt = _settings.find(settingName);
		if (settingIt == _settings.end()) {
			throw std::out_of_range("No setting with name \"" + settingName + "\" found");
		}
		const auto* setting = dynamic_cast<Setting<DataType>*>(settingIt->second);

		if (!setting) {
			throw std::invalid_argument(
				"Cannot set value of type \"" + easyqt::typeName<DataType>() + " for setting \"" +
				settingName + "\" with type \"" + settingIt->second->typeName() + "\""
			);
		}

		setting->setSavedValue(value);
	}
	template
	bool SettingsDialog::settingValue<bool>(const std::string&) const;

	template
	int SettingsDialog::settingValue<int>(const std::string&) const;

	template
	double SettingsDialog::settingValue<double>(const std::string&) const;

	template
	std::string SettingsDialog::settingValue<std::string>(const std::string&) const;

	template
	std::filesystem::path SettingsDialog::settingValue<std::filesystem::path>(const std::string&) const;
}
