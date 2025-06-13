#include <QLabel>

#include "Client/GUI/UI/LoginScreen.hpp"
#include "Client/GUI/BackManager.hpp"

#include "Client/Common/ipc/Header.hpp"
#include "Client/Common/ipc/Helper.hpp"

#include "Common/Container/ProcessUnit.hpp"
#include "Common/Log/Manager.hpp"

namespace ui
{
    LoginScreen::LoginScreen(QWidget *parent)
        : QDialog(parent),
        m_layout(new QVBoxLayout()),
        m_entry(new QLineEdit()),
        m_button(new QPushButton("Submit")),
        m_progress(new QProgressBar()),
        Logger(logger::Manager::newLogger("LoginScreen"))
    {
        QLabel *label = new QLabel("Login:");

        connect(m_button, &QPushButton::clicked, this, &LoginScreen::onSubmit);
        connect(this, &LoginScreen::requestConnection, BackManager::Instance(), &BackManager::startConnection, Qt::QueuedConnection);

        m_progress->setRange(0, 4);

        m_layout->addWidget(label);
        m_layout->addWidget(m_entry);
        m_layout->addWidget(m_button);
        setLayout(m_layout);

        setWindowTitle("FIX4.2 Login");
    }

    void LoginScreen::backNotification(net::Buffer _buffer)
    {
        ipc::Header header;

        Logger->log<logger::Level::Verbose>("New notification from backend");
        _buffer >> header;
        Logger->log<logger::Level::Debug>("Message received: (MsgType: ", static_cast<int>(header.MsgType), ", length: ", header.BodySize, ")");
        if (header.MsgType == ipc::MessageType::Status) {
            uint8_t status;
            PUStatus pustatus;

            _buffer >> status;
            pustatus = static_cast<PUStatus>(status);
            Logger->log<logger::Level::Info>("Back status: ", static_cast<int>(pustatus));
            m_progress->setValue(status);
            if (pustatus == PUStatus::Running) {
                const std::string &name = m_entry->text().toStdString();

                Logger->log<logger::Level::Info>("Sending request to login to the backend");
                BackManager::Instance()->send(ipc::Helper::Logon(name, 1));
            }
        } else if (header.MsgType == ipc::MessageType::Logon) {
            m_progress->setValue(4);
        } else {
            Logger->log<logger::Level::Warning>("Unknow signal from back during login setup");
        }
    }

    void LoginScreen::onSubmit()
    {
        m_entry->setEnabled(false);
        m_button->setEnabled(false);
        m_layout->addWidget(m_progress);

        emit requestConnection();
        connect(BackManager::Instance(), &BackManager::received, this, &LoginScreen::backNotification);
    }
}