#pragma once

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

#include "Common/Log/ILogger.hpp"

namespace ui
{
    class LoginScreen : public QDialog
    {
        Q_OBJECT

        public:
            LoginScreen(QWidget *parent = nullptr);
            ~LoginScreen() = default;

        signals:
            void requestConnection();

        private:
            void onSubmit();

            QLineEdit *m_entry = nullptr;
            QPushButton *m_button = nullptr;

            std::unique_ptr<logger::ILogger> Logger = nullptr;
    };
}