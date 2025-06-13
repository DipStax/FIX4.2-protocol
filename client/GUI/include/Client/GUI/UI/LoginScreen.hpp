#pragma once

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProgressBar>

#include "Common/Network/Buffer.hpp"
#include "Common/Log/ILogger.hpp"

namespace ui
{
    class LoginScreen : public QDialog
    {
        Q_OBJECT

        public:
            LoginScreen(QWidget *parent = nullptr);
            ~LoginScreen() = default;

        public slots:
            void backNotification(net::Buffer _buffer);

        signals:
            void requestConnection();

        private:
            void onSubmit();

            QVBoxLayout *m_layout = nullptr;

            QLineEdit *m_entry = nullptr;
            QPushButton *m_button = nullptr;
            QProgressBar *m_progress = nullptr;

            std::unique_ptr<logger::ILogger> Logger = nullptr;

    };
}