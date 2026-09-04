#pragma once

#include <filesystem>
#include <qlistwidget.h>
#include <vector>

#include <QBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QStackedWidget>

#include "object.hxx"

class SettingsPage;
namespace easyqt {
        class SettingsDialog: public Object<QDialog> {
                Q_OBJECT
                public:
                        void loadFromFile(std::filesystem::path path);
                
                protected:
                        void initImpl() override;
                
                protected slots:
                        void onClicked(QAbstractButton* button);
                        void onPageSelected(QListWidgetItem* current, QListWidgetItem* previous);
                
                private:
                        std::vector<SettingsPage*> _pages;
                        QVBoxLayout* _layout;
                        QHBoxLayout* _settingsLayout;
                        QListWidget* _pageList;
                        QStackedWidget* _pageContainer;
                        QDialogButtonBox* _buttonBox;
        };
}
