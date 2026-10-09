#pragma once

#include <filesystem>
#include <vector>

#include <QBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QStackedWidget>

#include <pugixml.hpp>

#include "object.hxx"


enum class SettingType {
	Bool = 0,
	Int = 1,
	Double = 2,
	String = 3,
	Path = 4,
};

enum class SettingLevel {
	Basic = 0,
	Advanced = 1,
	Developer = 2,
};

namespace easyqt {
	class SettingsDialog;
}

class SettingBase: public QObject {
        Q_OBJECT
	public:
		friend class easyqt::SettingsDialog;
		friend class SettingsPage;
		
		SettingBase(const std::string& name, const std::string& label, SettingLevel level, pugi::xml_node node):
			_name(name), _label(label), _level(level), _node(node)
		{
		}


		// Returns this setting's name, which is used by the settings dialog to identify this setting.
		// It should be unique within the application and should only contain letters, numbers, and hyphens to separate words.
		// Examples: username, video-quality, addon-search-paths
		const std::string& name() const { return _name; }

		// Returns this setting's label, which is displayed to the user and does not have to be unique within the application.
		// Examples: Username, Video quality, Addon search paths
		const std::string& label() const { return _label; }

		
		const SettingLevel level() const { return _level; }
		
		const pugi::xml_node node() const { return _node; }
		
		virtual const SettingType type() const = 0;
		virtual const std::string& typeName() const = 0;
		virtual bool isDefault() const = 0;
		virtual bool isModified() const = 0;
		virtual bool isSaved() const = 0;

	signals:
		void changed(const std::string& name);

	protected:
		virtual QWidget* createWidget() = 0;
		virtual void parseValues(pugi::xml_node userValueNode) = 0;
                virtual void applyValue() = 0;
		virtual std::string savedValueString() const = 0;
		virtual void saveValue() = 0;

		virtual void resetToDefaultValue() = 0;
		virtual void resetToSavedValue() = 0;


		std::string _name = "";
		std::string _label = "";
		SettingLevel _level;
		pugi::xml_node _node;
};

class SettingsPage;

namespace easyqt {
        class SettingsDialog: public Object<QDialog> {
                Q_OBJECT
                public:
                        void setFactorySettingsPath(std::filesystem::path path) {
                                if (path != _factoryPath) {
                                        _factoryPath = path;
                                }
                                rebuild();
                        }
                        void writeSettingsToFile();
                        void rebuild();

			template<typename DataType>
			DataType settingValue(const std::string& settingName) const;

			template<typename DataType>
			void setSettingValue(const std::string& settingName, const DataType& value);
                
                protected:
                        void initImpl() override;

                        void applySettings();
			void resetSettingsToDefault();
			void resetSettingsToSaved();
                        void updateButtonStates();
                
                protected slots:
                        void onClicked(QAbstractButton* button);
                        void onPageSelected(QListWidgetItem* current, QListWidgetItem* previous);
                        void onSettingChanged(const std::string& settingName);
                
                private:
                        // Path to load default settings from when no user settings file is found
                        std::filesystem::path _factoryPath;

                        // Path to save / load user settings to / from
                        std::filesystem::path _path;
                        
                        std::vector<SettingsPage*> _pages;
                        std::map<std::string, SettingBase*> _settings;

                        pugi::xml_document _settingsStorage;
                        QVBoxLayout* _layout;
                        QHBoxLayout* _settingsLayout;
                        QListWidget* _pageList;
                        QStackedWidget* _pageContainer;
                        QDialogButtonBox* _buttonBox;

                        int _numUnsavedSettings = 0;
                        int _numNonDefaultSettings = 0;
        };
}
