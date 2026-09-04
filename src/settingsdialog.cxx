#include <filesystem>
#include <qcheckbox.h>
#include <qobject.h>
#include <qwidget.h>
#include <stdexcept>
#include <string>

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>

#include <pugixml.hpp>

#include "logging.hxx"
#include "settingsdialog.hxx"
#include "utils.hxx"

enum class SettingType {
	Bool = 0,
	Int = 1,
	Double = 2,
	String = 3,
	Path = 4,
	PathList = 5,
};

enum class SettingLevel {
	Basic = 0,
	Advanced = 1,
	Developer = 2,
};

static std::map<std::string, SettingType> typeMapping = {
	{"bool", SettingType::Bool},
	{"int", SettingType::Int},
	{"double", SettingType::Double},
	{"string", SettingType::String},
	{"path", SettingType::Path},
	{"pathlist", SettingType::PathList},
};

static std::map<std::string, SettingLevel> levelMapping = {
	{"basic", SettingLevel::Basic},
	{"advanced", SettingLevel::Advanced},
	{"developer", SettingLevel::Developer},
};

class SettingBase: public QObject {
	public:
		friend class SettingsPage;
		
		SettingBase(const std::string& name, const std::string& label, SettingType type, SettingLevel level, pugi::xml_node node):
			_name(name), _label(label), _type(type), _level(level), _node(node)
		{
		}


		// Returns this setting's name, which is used by the settings dialog to identify this setting.
		// It should be unique within the application and should only contain letters, numbers, and hyphens to separate words.
		// Examples: username, video-quality, addon-search-paths
		const std::string& name() const { return _name; }

		// Returns this setting's label, which is displayed to the user and does not have to be unique within the application.
		// Examples: Username, Video quality, Addon search paths
		const std::string& label() const { return _label; }

		const SettingType type() { return _type; }

		const SettingLevel level() { return _level; }
		
		const pugi::xml_node node() { return _node; }

		virtual QWidget* createWidget() = 0;


	protected:
		std::string _name = "";
		std::string _label = "";
		SettingType _type;
		SettingLevel _level;
		pugi::xml_node _node;
};

template<typename DataType>
class Setting: public SettingBase {
	public:
		friend class SettingsPage;

		Setting(const std::string& name, const std::string& label, SettingType type, SettingLevel level, const DataType& defaultValue, pugi::xml_node node):
			SettingBase(name, label, type, level, node), _defaultValue(defaultValue), _currentValue(defaultValue), _savedValue(_defaultValue)
		{
		}

		// Returns true if the setting is set to the default value
		bool isDefault() const {
			return _currentValue == _defaultValue;
		}

		// Returns true if the setting has been modified by the user and has not been saved yet
		bool isModified() const {
			return _currentValue != _savedValue;
		}

		// Returns true if the setting has been saved.
		bool isSaved() const {
			return _currentValue == _savedValue;
		}

		void setSavedValue(DataType value) {
			_savedValue = value;
		}

		DataType currentValue() const { return _currentValue; }
		DataType defaultValue() const { return _defaultValue; }
		DataType savedValue() const { return _savedValue; }

		virtual QWidget* createWidget() override {
			LOG(WARNING, "Cannot create setting widget for type '" << easyqt::typeName<DataType>() << "': not implemented");
			return nullptr;
		};

	signals:
		void changed(const std::string& name);

	protected:
		DataType _defaultValue, _currentValue, _savedValue;
};

template<>
QWidget* Setting<bool>::createWidget() {
	QCheckBox* widget = new QCheckBox();
	widget->setChecked(_savedValue);

	return widget;
}

template<>
QWidget* Setting<int>::createWidget() {
	QSpinBox* widget = new QSpinBox();
	widget->setValue(_savedValue);

	return widget;
}

template<>
QWidget* Setting<double>::createWidget() {
	QDoubleSpinBox* widget = new QDoubleSpinBox();
	widget->setValue(_savedValue);

	return widget;
}

template<>
QWidget* Setting<std::string>::createWidget() {
	QLineEdit* widget = new QLineEdit();
	widget->setText(_savedValue.c_str());

	return widget;
}

template<>
QWidget* Setting<std::filesystem::path>::createWidget() {
	QPushButton* widget = new QPushButton();
	widget->setText(_savedValue.c_str());

	return widget;
}

template<>
QWidget* Setting<std::vector<std::filesystem::path> >::createWidget() {
	QListWidget* widget = new QListWidget();
	//widget->setText(_savedValue.c_str());

	return widget;
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


		template<typename ValueType>
		void addSetting(
			const std::string& name, const std::string& label,
			SettingType type, SettingLevel level, ValueType defaultValue, pugi::xml_node node
		) {
			std::shared_ptr<Setting<ValueType> > setting = std::make_shared<Setting<ValueType> >(name, label, type, level, defaultValue, node);
			_settings.push_back(setting);
			_lastrow += 1;
			_layout.addWidget(new QLabel(label.c_str()), _lastrow, 0, Qt::AlignTop);
			_layout.addWidget(setting->createWidget(), _lastrow, 1, Qt::AlignTop);
		}
	
	private:
		std::string _name = "";
		std::string _label = "";
		QGridLayout _layout;
		int _lastrow = -1;
		std::vector< std::shared_ptr<QObject> > _settings;
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
		_buttonBox->button(QDialogButtonBox::RestoreDefaults)->setEnabled(false);
		_buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
		_buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
		_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		_layout->addWidget(_buttonBox);
		QObject::connect(_buttonBox, &QDialogButtonBox::clicked, this, &SettingsDialog::onClicked);
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
		if (role == QDialogButtonBox::Close) {
			close();
		}
	}

	void SettingsDialog::loadFromFile(std::filesystem::path file) {
		std::string path = getResourcePath(file);
		pugi::xml_document xml;
		pugi::xml_parse_result result = xml.load_file(path.c_str());
		if (!result) {
			LOG(ERROR, "Failed loading settings dialog file '" << file << "': " << result.description() << " !");
			return;
		}
		
		pugi::xml_node settingsNode = xml.child("settings");
		for (pugi::xml_node categoryNode: settingsNode.children("category")) {
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
				if (settingName.empty()) {
					LOG(ERROR, "Cannot add setting without name");
					continue;
				}

				if (settingLabel.empty()) {
					LOG(WARN, "Adding setting with empty label, using name instead");
					settingLabel = settingName;
				} else {
					LOG(DEBUG, "Adding setting with label " << std::quoted(settingLabel));
				}

				std::string typeString = settingNode.child_value("type");
				std::string levelString = settingNode.child_value("level");

				SettingType type;
				try {
					type = typeMapping.at(typeString);
				} catch (std::out_of_range& e) {
					LOG(ERROR, "Cannot add setting with unknown type " << std::quoted(typeString));
					continue;
				}
				SettingLevel level;
				try {
					level = levelMapping.at(levelString);
				} catch (std::out_of_range& e) {
					LOG(WARN, "Unknown setting level " << std::quoted(levelString) << "; falling back to " << std::quoted("basic"));
					level = SettingLevel::Basic;
				}

				pugi::xml_node defaultValueNode = settingNode.child("default");
				bool hasDefaultValue = !defaultValueNode.empty();
				if (!hasDefaultValue) {
					LOG(ERROR, "Cannot add setting without default value");
					continue;
				}
				std::string defaultValueString = defaultValueNode.text().as_string();
				

				switch (type) {
					case SettingType::Bool: {
						bool defaultValue = false;
						if (defaultValueString == "true" || defaultValueString == "1") {
							defaultValue = true;
						} else if (defaultValueString == "false" || defaultValueString == "0") {
							defaultValue = false;
						} else {
							LOG(ERROR, "Invalid default boolean value " << std::quoted(defaultValueString));
						}
						page->addSetting(settingName, settingLabel, type, level, defaultValue, settingNode);
						break;
					}

					case SettingType::Int: {
						int defaultValue = 0;
						try {
							defaultValue = std::stoi(defaultValueString);
						}
						catch (const std::invalid_argument& e) {
							LOG(ERROR, "Invalid default integer value " << std::quoted(defaultValueString));
						}
						catch (const std::out_of_range& e) {
							LOG(ERROR, "Default integer value out of range " << std::quoted(defaultValueString));
						}
						page->addSetting(settingName, settingLabel, type, level, defaultValue, settingNode);
						break;
					}

					case SettingType::Double: {
						double defaultValue = 0.0;
						try {
							defaultValue = std::stod(defaultValueString);
						}
						catch (const std::invalid_argument& e) {
							LOG(ERROR, "Invalid default double value " << std::quoted(defaultValueString));
						}
						catch (const std::out_of_range& e) {
							LOG(ERROR, "Default double value out of range " << std::quoted(defaultValueString));
						}
						page->addSetting(settingName, settingLabel, type, level, defaultValue, settingNode);
						break;
					}

					case SettingType::String: {
						std::string defaultValue(defaultValueString);
						page->addSetting(settingName, settingLabel, type, level, defaultValue, settingNode);
						break;
					}

					case SettingType::Path: {
						std::filesystem::path defaultValue(defaultValueString);
						page->addSetting(settingName, settingLabel, type, level, defaultValue, settingNode);
						break;
					}

					case SettingType::PathList: {
						std::vector<std::filesystem::path> defaultValue;
						std::stringstream splitPaths(defaultValueString);
						std::string path;
						while (std::getline(splitPaths, path, ';')) {
							if (!path.empty()) {
								defaultValue.push_back(path);
							}
						}
						page->addSetting(settingName, settingLabel, type, level, defaultValue, settingNode);
						break;
					}
					default: {
						LOG(ERROR, "Unsupported setting type '" << typeString);
					}
				}
			}
		}
		
		_pageList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		const int width = _pageList->sizeHintForColumn(0) + 2 * _pageList->frameWidth();
		_pageList->setFixedWidth(width);
	}
}
