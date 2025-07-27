#include "Server/Core/ProcessUnit/User/Logon.hpp"

#include "Common/Utils.hpp"
#include "Common/Message/Logon.hpp"
#include "Common/Message/Tag.hpp"
#include "Common/Log/Manager.hpp"

namespace pu::user
{
    LogonHandler::LogonHandler(InputNetworkOutput &_tcp_output)
        : AInputProcess<InputType>("Server/User/Logon"),
        m_tcp_output(_tcp_output)
    {
    }

    void LogonHandler::onInput(InputType _input)
    {
        m_tp.enqueue([this, _input] () mutable {
            process(_input);
        });
    }

    bool LogonHandler::process(InputType &_input)
    {
        Logger->log<logger::Level::Info>("Processing message...");
        fix::Logon logon;
        std::pair<bool, fix::Reject> reject = fix::Logon::Verify(_input.Message);

        if (reject.first) {
            if (reject.second.contains(fix::Tag::Text))
                Logger->log<logger::Level::Info>("Header verification failed: (", reject.second.get(fix::Tag::RefTagId), ") ", reject.second.get(fix::Tag::Text));
            else
                Logger->log<logger::Level::Warning>("Header verification failed for unknown reason");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            Logger->log<logger::Level::Debug>("Reject moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        } else if (_input.Client->isLoggedin()) {
            Logger->log<logger::Level::Warning>("Client (", *(_input.Client), ") already connected");
            reject.second.set45_refSeqNum(_input.Message.at(fix::Tag::MsqSeqNum));
            reject.second.set58_text("Client already logged in");
            Logger->log<logger::Level::Debug>("Reject from (", *(_input.Client), ") moving to TCP output");
            m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(reject.second));
            return false;
        }

        _input.Client->login(_input.Message.at(fix::Tag::SenderCompId));
        _input.Client->setSeqNumber(utils::to<size_t>(_input.Message.at(fix::Tag::MsqSeqNum)) + 1);

        InternalClient::HeartBeatInfo &hb_info = _input.Client->getHeartBeatInfo();

        hb_info.Since = std::chrono::system_clock::now();
        hb_info.Elapsing = std::max(1.f, std::min(PU_LOGON_HB_MAX_TO, utils::to<float>(_input.Message.at(fix::Tag::HeartBtInt))));

        Logger->log<logger::Level::Debug>("Set heartbeat int at: ", hb_info.Elapsing);
        Logger->log<logger::Level::Info>("Client set as logged in as: (", *(_input.Client), ")");
        logon.set98_EncryptMethod("0");
        logon.set108_HeartBtInt(std::to_string(static_cast<int>(hb_info.Elapsing)));
        Logger->log<logger::Level::Debug>("Reply to (", *(_input.Client), ") moving to TCP output");
        m_tcp_output.append(_input.Client, _input.ReceiveTime, std::move(logon));
        return true;
    }
}