#pragma once

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProgressBar>

#include "Client/Shared/IPC/Message/Logon.hpp"

#include "Shared/ProcessUnit/ProcessUnit.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Buffer.hpp"

namespace ui::screen
{
    class Login : public QDialog
    {
        Q_OBJECT

        public:
            Login(QWidget *parent = nullptr);
            ~Login() = default;

        public slots:
            void backNotifyStatus(PUStatus _status);
            void backNotifyLogon(ipc::msg::Logon _logon);

        signals:
            void requestConnection();

        private:

            void onSubmit();

            QVBoxLayout *m_layout = nullptr;

            QLineEdit *m_uid_entry = nullptr;
            QLineEdit *m_hb_entry = nullptr;
            QLineEdit *m_seqnum_entry = nullptr;

            QPushButton *m_button = nullptr;
            QProgressBar *m_progress = nullptr;

            std::unique_ptr<logger::ILogger> Logger = nullptr;

    };
}