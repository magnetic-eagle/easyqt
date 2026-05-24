#pragma once

#include <filesystem>

#include <QBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTabWidget>

#include "object.hxx"

namespace easyqt {
        class SettingsDialog: public QDialog, public Object {
                public:
                        void loadFromFile(std::filesystem::path path);
                
                protected:
                        virtual void initImpl() override;
                
                private:
                        QTabWidget* _settingsTabs;
                        QVBoxLayout* _layout;
                        QDialogButtonBox* _buttonBox;
        };
}
