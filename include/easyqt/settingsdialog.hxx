#pragma once

#include <filesystem>

#include <QBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTabWidget>

#include "object.hxx"

namespace easyqt {
        class SettingsDialog: public Object<QDialog> {
                Q_OBJECT
                public:
                        void loadFromFile(std::filesystem::path path);
                
                protected:
                        void initImpl() override;
                
                protected slots:
                        void onClicked(QAbstractButton* button);
                
                private:
                        QTabWidget* _settingsTabs;
                        QVBoxLayout* _layout;
                        QDialogButtonBox* _buttonBox;
        };
}
