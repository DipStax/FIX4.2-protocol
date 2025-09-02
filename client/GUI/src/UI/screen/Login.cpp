#include <QLabel>
#include <QIntValidator>
#include <QDoubleValidator>

#include "Client/GUI/UI/screen/Login.hpp"
#include "Client/GUI/BackManager.hpp"
#include "Client/GUI/InitiatorManager.hpp"

#include "Client/Shared/IPC/Header.hpp"
#include "Client/Shared/IPC/Helper.hpp"
#include "Client/Shared/IPC/Message/Message.hpp"

#include "Shared/Log/Manager.hpp"

namespace ui::screen
{
    Login::Login(QWidget *parent)
        : QDialog(parent),
        m_layout(new QVBoxLayout()),
        m_uid_entry(new QLineEdit()),
        m_hb_entry(new QLineEdit()),
        m_seqnum_entry(new QLineEdit()),
        m_button(new QPushButton("Submit")),
        m_progress(new QProgressBar()),
        Logger(logger::Manager::newLogger("Screen/Login"))
    {
        m_hb_entry->setValidator(new QDoubleValidator(0, 100, 2));
        m_hb_entry->setText("5.00");
        m_seqnum_entry->setValidator(new QIntValidator(1, 255));
        m_seqnum_entry->setText("1");
        m_progress->setRange(0, 4);

        m_layout->addWidget(new QLabel("Login:"));
        m_layout->addWidget(m_uid_entry);
        m_layout->addWidget(new QLabel("HeartBeat:"));
        m_layout->addWidget(m_hb_entry);
        m_layout->addWidget(new QLabel("Seqence Number:"));
        m_layout->addWidget(m_seqnum_entry);
        m_layout->addWidget(m_button);
        setLayout(m_layout);

        connect(m_button, &QPushButton::clicked, this, &Login::onSubmit);
        // connect(this, &Login::requestConnection, BackManager::Instance(), &BackManager::startConnection, Qt::QueuedConnection);
        connect(this, &Login::requestConnection, InitiatorManager::Instance(), &InitiatorManager::startConnection, Qt::QueuedConnection);

        setWindowTitle("FIX4.2 Login");
    }

    // void Login::backNotifyStatus(PUStatus _status)
    // {
    //     ipc::msg::Logon logon{};

    //     m_progress->setValue(static_cast<uint8_t>(_status));
    //     switch (_status) {
    //         case PUStatus::Initialize: m_progress->setValue(1);
    //             break;
    //         case PUStatus::Running:
    //             m_progress->setValue(2);
    //             logon = {
    //                 m_uid_entry->text().toStdString(),
    //                 static_cast<uint32_t>(std::stoul(m_seqnum_entry->text().toStdString())),
    //                 std::stof(m_hb_entry->text().toStdString())
    //             };

    //             Logger->log<logger::Level::Info>("Sending request to login to the backend");
    //             disconnect(BackManager::Instance(), &BackManager::received_Status, this, &Login::backNotifyStatus);
    //             connect(BackManager::Instance(), &BackManager::received_Logon, this, &Login::backNotifyLogon);
    //             BackManager::Instance()->send(ipc::Helper::Logon(logon));
    //             break;
    //         case PUStatus::Unknown:
    //         case PUStatus::Stop:
    //             Logger->log<logger::Level::Warning>("Backend isn't start proprely");
    //             break;
    //     }
    // }

    // void Login::backNotifyLogon(ipc::msg::Logon _logon)
    // {
    //     Logger->log<logger::Level::Info>("Received validation of logon with: { userId: ", _logon.UserId, ", seqnum: ", _logon.SeqNum, ", heartbeat: ", _logon.HeartBeat, " }");
    //     m_progress->setValue(4);
    //     accept();
    // }

    void Login::sendIdentification()
    {
        ipc::msg::IdentifyFront identifier;

        identifier.apiKey = "api-key"; // todo use configuration
        Logger->log<logger::Level::Info>("Sending request to identify frontend to initiator");
        connect(InitiatorManager::Instance(), &InitiatorManager::received_IdentifyFront, this, &Login::validatedIdentification);
        InitiatorManager::Instance()->send(ipc::Helper::FrontIdentification(identifier));
    }

    void Login::validatedIdentification(ipc::msg::IdentifyFront _identify)
    {
        m_progress->setValue(1);
        disconnect(InitiatorManager::Instance(), &InitiatorManager::received_IdentifyFront, this, &Login::validatedIdentification);
    }

    void Login::onSubmit()
    {
        m_uid_entry->setEnabled(false);
        m_button->setEnabled(false);
        m_layout->addWidget(m_progress);

        connect(InitiatorManager::Instance(), &InitiatorManager::connectionReady, this, &Login::sendIdentification);
        emit requestConnection();
        // connect(BackManager::Instance(), &BackManager::received_Status, this, &Login::backNotifyStatus);
    }
}