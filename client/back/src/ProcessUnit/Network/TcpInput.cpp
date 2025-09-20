#include "Client/Back/ProcessUnit/Network/TcpInput.hpp"
#include "Client/Back/User.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Utils/Utils.hpp"

namespace pu
{
    TcpInputNetwork::TcpInputNetwork(Socket _server, UnparsedMessageQueue &_output, StringOutputQueue &_error)
        : AProcessUnitBase("Back/NET/TCP-Input"),
        m_server(_server), m_output(_output), m_error(_error)
    {
        m_selector.timeout(1000);
        Logger->log<logger::Level::Verbose>("Server socket added to selector");
    }

    void TcpInputNetwork::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Launching process unit runtime");
        int error = 0;

        m_selector.client(m_server);
        while (!_st.stop_requested()) {
            std::vector<Socket> clients = m_selector.pull();

            if (!clients.empty()) {
                std::vector<std::byte> bytes = clients[0]->receive(2048, error);
                std::string data(reinterpret_cast<const char*>(bytes.data()), + bytes.size());

                if (error == 0) {
                    Logger->log<logger::Level::Fatal>("Server socket disonnected");
                    // todo
                    continue;
                }
                Logger->log<logger::Level::Info>("New message received: ", data);
                if (User::Instance().isLogin())
                    User::Instance().nextSeqNumber();
                buildHeader(data);
            }
        }
    }

    void TcpInputNetwork::buildHeader(const std::string &_data)
    {
        fix42::Header header{};
        std::unordered_set<uint16_t> tagset{};
        std::vector<std::string> split = utils::split<'\1'>(_data);
        fix::MapMessage map;
        bool header_complet = false;
        uint32_t checksum = 0;
        uint32_t last_checksum = 0;

        for (const std::string &result : split) {
            std::vector<std::string> kvsplit = utils::split<'='>(result);

            last_checksum = CountCheckSum(result);
            checksum += last_checksum;
            if (kvsplit.size() != 2) {
                Logger->log<logger::Level::Error>("No '=' separator found, rejecting message");
                fix42::msg::SessionReject reject{};

                if (tagset.contains(fix42::tag::MsgSeqNum))
                    reject.get<fix42::tag::RefSeqNum>().Value = header.get<fix42::tag::MsgSeqNum>().Value;
                else
                    reject.get<fix42::tag::RefSeqNum>().Value = User::Instance().getSeqNumber();
                if (tagset.contains(fix42::tag::MsgSeqNum))
                reject.get<fix42::tag::RefMsgType>().Value = header.getPositional<fix42::tag::MsgType>().Value;
                else
                    Logger->log<logger::Level::Warning>("Unable to find message type in header, reject has empty RefMsgType");
                reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::UndefineTag;
                reject.get<fix42::tag::Text>().Value = "Unable to parse header key-value";
                m_error.append(std::chrono::system_clock::now(), fix42::msg::SessionReject::Type, std::move(reject.to_string()));
                return;
            }
            if (!header_complet) {
                Logger->log<logger::Level::Verbose>("Inserting: ", kvsplit[0], " = ", kvsplit[1]);
                xstd::Expected<bool, fix::RejectError> error = header.try_insert(kvsplit[0], kvsplit[1]);

                if (error.has_error()) {
                    Logger->log<logger::Level::Error>("Error during insertion of: ", kvsplit[0], " = ", kvsplit[1], ": ", error.error().Message);
                    m_error.append(std::chrono::system_clock::now(), fix42::msg::SessionReject::Type, std::move(BuildRejectFromError(error.error(), tagset, header).to_string()));
                    return;
                }
                if (!error.value()) {
                    std::optional<fix::RejectError> reject = verifyHeader(header, tagset);

                    if (reject.has_value()) {
                        Logger->log<logger::Level::Info>("header verification failed: ", reject.value().Message);
                        m_error.append(std::chrono::system_clock::now(), fix42::msg::SessionReject::Type, std::move(BuildRejectFromError(reject.value(), tagset, header).to_string()));
                        // send to output
                        return;
                    }
                    Logger->log<logger::Level::Info>("Header verification completed sucessfully");
                    header_complet = true;
                    map.emplace_back(kvsplit[0], kvsplit[1]);
                    continue;
                }
                tagset.emplace(static_cast<uint16_t>(std::stoi(kvsplit[0])));
            } else {
                map.emplace_back(kvsplit[0], kvsplit[1]);
            }
        }
        std::optional<fix::RejectError> reject = verifyCheckSum(checksum - last_checksum, map);

        if (reject.has_value()) {
            Logger->log<logger::Level::Info>("Checksum verification failed: ", reject.value().Message);
            m_error.append(std::chrono::system_clock::now(), fix42::msg::SessionReject::Type, std::move(BuildRejectFromError(reject.value(), tagset, header).to_string()));
            return;
        }
        Logger->log<logger::Level::Debug>("Finish parsing the input message");
        m_output.append(std::chrono::system_clock::now(), std::move(header), std::move(map));
    }

    std::optional<fix::RejectError> TcpInputNetwork::verifyHeader(const fix42::Header &_header, const std::unordered_set<uint16_t> &_tagset)
    {
        std::optional<fix::RejectError> error = _header.verify();

        if (error.has_value())
            return error;
        return TcpInputNetwork::HeaderVerifyPresence<fix42::tag::SenderCompId, fix42::tag::TargetCompId, fix42::tag::MsgSeqNum, fix42::tag::SendingTime>(_tagset);
    }

    fix42::msg::SessionReject TcpInputNetwork::BuildRejectFromError(const fix::RejectError &_error, const std::unordered_set<uint16_t> &_tagset, const fix42::Header &_header)
    {
        fix42::msg::SessionReject reject{};

        if (_tagset.contains(fix42::tag::MsgSeqNum))
            reject.get<fix42::tag::RefSeqNum>().Value = _header.get<fix42::tag::MsgSeqNum>().Value;
        else
            reject.get<fix42::tag::RefSeqNum>().Value = User::Instance().getSeqNumber();
        if (_tagset.contains(fix42::tag::MsgSeqNum))
            reject.get<fix42::tag::RefMsgType>().Value = _header.getPositional<fix42::tag::MsgType>().Value;
        else
            Logger->log<logger::Level::Warning>("Unable to find message type in header, reject has empty RefMsgType");
        reject.get<fix42::tag::SessionRejectReason>().Value = static_cast<fix42::RejectReasonSession>(_error.Reason);
        reject.get<fix42::tag::RefTagId>().Value = _error.Tag;
        reject.get<fix42::tag::Text>().Value = _error.Message;
        return reject;
    }

    uint32_t TcpInputNetwork::CountCheckSum(const std::string &_data)
    {
        uint32_t count = 0;

        for (const char _c : _data)
            count += static_cast<uint32_t>(_c);
        return count + 1;
    }

    std::optional<fix::RejectError> TcpInputNetwork::verifyCheckSum(uint32_t _checksum, fix::MapMessage &_map) const
    {
        uint8_t checksum = static_cast<uint8_t>(_checksum % 256);
        std::optional<fix::RejectError> reject = std::nullopt;
        uint8_t out_checksum = 0;
        std::pair<std::string, std::string> pair ;

        if (_map.size() == 0)
            return fix::RejectError{ fix::RejectError::ReqTagMissing, "Last tag should be CheckSum", fix42::tag::CheckSum };
        pair = _map.back();
        if (pair.first == "10") {
            reject = from_FIX(pair.second, out_checksum);

            if (reject.has_value())
                return reject;
            if (out_checksum != checksum) {
                Logger->log<logger::Level::Info>("Expected: ", (int)checksum, ", got: ", (int)out_checksum);
                return fix::RejectError{ fix::RejectError::ValueOORange, "Miss match on checksum", fix42::tag::CheckSum };
            }
            _map.pop_back();
            return std::nullopt;
        } else {
            return fix::RejectError{ fix::RejectError::ReqTagMissing, "Last tag should be CheckSum", fix42::tag::CheckSum };
        }
    }
}