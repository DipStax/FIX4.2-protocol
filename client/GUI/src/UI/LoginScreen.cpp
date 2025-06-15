#include <QLabel>
#include <QIntValidator>
#include <QDoubleValidator>

#include "Client/GUI/UI/LoginScreen.hpp"
#include "Client/GUI/BackManager.hpp"

#include "Client/Common/IPC/Header.hpp"
#include "Client/Common/IPC/Helper.hpp"

#include "Common/Container/ProcessUnit.hpp"
#include "Common/Log/Manager.hpp"

namespace ui
{
    LoginScreen::LoginScreen(QWidget *parent)
        : QDialog(parent),
        m_layout(new QVBoxLayout()),
        m_uid_entry(new QLineEdit()),
        m_hb_entry(new QLineEdit()),
        m_seqnum_entry(new QLineEdit()),
        m_button(new QPushButton("Submit")),
        m_progress(new QProgressBar()),
        Logger(logger::Manager::newLogger("LoginScreen"))
    {
        QLabel *label_login = new QLabel("Login:");
        QLabel *label_hb = new QLabel("HeartBeat:");
        QLabel *label_seqnum = new QLabel("Seqence Number:");
        QDoubleValidator *validator_hb = new QDoubleValidator(0, 100, 2);
        QIntValidator *validator_seqnum = new QIntValidator(1, 255);

        m_hb_entry->setValidator(validator_hb);
        m_hb_entry->setText("5.00");
        m_seqnum_entry->setValidator(validator_seqnum);
        m_seqnum_entry->setText("1");
        m_progress->setRange(0, 4);

        m_layout->addWidget(label_login);
        m_layout->addWidget(m_uid_entry);
        m_layout->addWidget(label_hb);
        m_layout->addWidget(m_hb_entry);
        m_layout->addWidget(label_seqnum);
        m_layout->addWidget(m_seqnum_entry);
        m_layout->addWidget(m_button);
        setLayout(m_layout);

        connect(m_button, &QPushButton::clicked, this, &LoginScreen::onSubmit);
        connect(this, &LoginScreen::requestConnection, BackManager::Instance(), &BackManager::startConnection, Qt::QueuedConnection);

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
                ipc::msg::Logon logon{
                    m_uid_entry->text().toStdString(),
                    static_cast<uint32_t>(std::stoul(m_seqnum_entry->text().toStdString())),
                    std::stof(m_hb_entry->text().toStdString())
                };

                Logger->log<logger::Level::Info>("Sending request to login to the backend");
                BackManager::Instance()->send(ipc::Helper::Logon(logon));
                m_progress->setValue(3);
            }
        } else if (header.MsgType == ipc::MessageType::Logon) {
            ipc::msg::Logon logon;

            _buffer >> logon;
            Logger->log<logger::Level::Info>("Received validation of logon with: { userId: ", logon.UserId, ", seqnum: ", logon.SeqNum, ", heartbeat: ", logon.HeartBeat, " }");
            m_progress->setValue(4);
        } else {
            Logger->log<logger::Level::Warning>("Unknow signal from back during login setup");
        }
    }

    void LoginScreen::onSubmit()
    {
        m_uid_entry->setEnabled(false);
        m_button->setEnabled(false);
        m_layout->addWidget(m_progress);

        emit requestConnection();
        connect(BackManager::Instance(), &BackManager::received, this, &LoginScreen::backNotification);
    }
}