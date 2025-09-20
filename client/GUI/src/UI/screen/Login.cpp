#include <QLabel>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QMessageBox>

#include "Client/GUI/UI/screen/Login.hpp"
#include "Client/GUI/BackManager.hpp"
#include "Client/GUI/InitiatorManager.hpp"
#include "Client/GUI/Config.hpp"

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
        m_hb_entry->setValidator(new QIntValidator(1, 100));
        m_hb_entry->setText("5");
        m_seqnum_entry->setValidator(new QIntValidator(1, 255));
        m_seqnum_entry->setText("1");
        m_progress->setRange(0, 7);
        m_progress->hide();

        m_layout->addWidget(new QLabel("Login:"));
        m_layout->addWidget(m_uid_entry);
        m_layout->addWidget(new QLabel("HeartBeat:"));
        m_layout->addWidget(m_hb_entry);
        m_layout->addWidget(new QLabel("Seqence Number:"));
        m_layout->addWidget(m_seqnum_entry);
        m_layout->addWidget(m_button);
        m_layout->addWidget(m_progress);
        setLayout(m_layout);

        connect(m_button, &QPushButton::clicked, this, &Login::onSubmit);
        connect(this, &Login::requestInitiatorConnection, InitiatorManager::Instance(), &InitiatorManager::startConnection, Qt::QueuedConnection);
        connect(this, &Login::requestBackConnection, BackManager::Instance(), &BackManager::startConnection, Qt::QueuedConnection);
        setWindowTitle("FIX4.2 Login");
    }

    void Login::sendIdentification()
    {
        ipc::msg::AuthFrontToInitiator auth;

        auth.apikey = Configuration<config::Global>::Get().Config.ApiKey;
        auth.name = m_uid_entry->text().toStdString();
        Logger->log<logger::Level::Info>("Sending request to identify frontend to initiator");
        m_progress->setValue(1);
        connect(InitiatorManager::Instance(), &InitiatorManager::received_Reject, this, &Login::invalidIdentification);
        connect(InitiatorManager::Instance(), &InitiatorManager::received_IdentifyFront, this, &Login::validatedIdentification);
        InitiatorManager::Instance()->send(ipc::Helper::Auth::FrontToInitiator(auth));
    }

    void Login::validatedIdentification(ipc::msg::AuthInitiatorToFront _identify)
    {
        std::ignore = _identify;

        m_progress->setValue(2);
        m_name = _identify.name;
        Logger->log<logger::Level::Info>("Authentication validated, waiting for token approval");
        connect(InitiatorManager::Instance(), &InitiatorManager::received_ValidationToken, this, &Login::tokenAuth);
        disconnect(InitiatorManager::Instance(), &InitiatorManager::received_IdentifyFront, this, &Login::validatedIdentification);
    }

    void Login::invalidIdentification(ipc::msg::Reject _reject)
    {
        disconnect(InitiatorManager::Instance(), &InitiatorManager::received_IdentifyFront, this, &Login::validatedIdentification);
        disconnect(InitiatorManager::Instance(), &InitiatorManager::received_Reject, this, &Login::invalidIdentification);
        QMessageBox::warning(this, "Error during login", QString("login rejected from Initiator:\n%1").arg(QString::fromStdString(_reject.message)));
        m_uid_entry->setEnabled(true);
        m_hb_entry->setEnabled(true);
        m_seqnum_entry->setEnabled(true);
        m_button->setEnabled(true);
        m_progress->hide();
    }

    void Login::tokenAuth(ipc::msg::InitiatorToFrontValidToken _token)
    {
        ipc::msg::FrontToBackValidToken token_valid{_token.token};
        Logger->log<logger::Level::Info>("Token approval received, connecting to backend");

        BackManager::Instance()->setTargetPort(_token.port);
        m_progress->setValue(3);
        emit requestBackConnection();

        disconnect(InitiatorManager::Instance(), &InitiatorManager::received_ValidationToken, this, &Login::tokenAuth);
        connect(BackManager::Instance(), &BackManager::received_TokenValidation, this, &Login::tokenValidated);

        Logger->log<logger::Level::Info>("Sending token validation to backend: ", token_valid);
        BackManager::Instance()->send(ipc::Helper::ValidationToken::FrontToBack(token_valid));
    }

    void Login::tokenValidated(ipc::msg::BackToFrontValidToken _token)
    {
        std::ignore = _token;

        m_progress->setValue(4);
        Logger->log<logger::Level::Info>("Backend synchronized");
        disconnect(BackManager::Instance(), &BackManager::received_TokenValidation, this, &Login::tokenValidated);
        connect(BackManager::Instance(), &BackManager::received_Status, this, &Login::backNotifyStatus);
    }

    void Login::backNotifyStatus(PUStatus _status)
    {
        ipc::msg::Logon logon{};

        m_progress->setValue(static_cast<uint8_t>(_status));
        switch (_status) {
            case PUStatus::Initialize: m_progress->setValue(5);
                break;
            case PUStatus::Running:
                m_progress->setValue(6);
                logon = {
                    m_name,
                    m_seqnum_entry->text().toUInt(),
                    m_hb_entry->text().toUInt()
                };

                Logger->log<logger::Level::Info>("Sending request to login to the backend");
                disconnect(BackManager::Instance(), &BackManager::received_Status, this, &Login::backNotifyStatus);
                connect(BackManager::Instance(), &BackManager::received_Logon, this, &Login::backNotifyLogon);
                BackManager::Instance()->send(ipc::Helper::Logon(logon));
                break;
            case PUStatus::Unknown:
            case PUStatus::Stop:
                Logger->log<logger::Level::Warning>("Backend isn't start proprely");
                break;
        }
    }

    void Login::backNotifyLogon(ipc::msg::Logon _logon)
    {
        Logger->log<logger::Level::Info>("Received validation of logon with: { userId: ", _logon.UserId, ", seqnum: ", _logon.SeqNum, ", heartbeat: ", _logon.HeartBeat, " }");
        m_progress->setValue(7);
        accept();
    }

    void Login::onSubmit()
    {
        m_uid_entry->setEnabled(false);
        m_hb_entry->setEnabled(false);
        m_seqnum_entry->setEnabled(false);
        m_button->setEnabled(false);
        m_progress->show();
        m_progress->setValue(0);

        if (InitiatorManager::Instance()->isConnectionReady())
            sendIdentification();
        else
            connect(InitiatorManager::Instance(), &InitiatorManager::connectionReady, this, &Login::sendIdentification);
        emit requestInitiatorConnection();
    }
}