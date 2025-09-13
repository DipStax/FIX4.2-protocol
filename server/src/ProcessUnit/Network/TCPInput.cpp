#include <future>

#include "Server/ProcessUnit/Network/TCPInput.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Utils/Utils.hpp"

namespace pu
{
    TCPInputNetwork::TCPInputNetwork(UnparsedMessageQueue &_output, StringOutputQueue &_error, uint32_t _port)
        : AProcessUnitBase("Server/NET/TCP-Input"),
        m_output(_output), m_error(_error)
    {
        (void)m_acceptor.listen(_port);
        (void)m_acceptor.setBlocking(false);
        m_selector.timeout(100);
        Logger->log<logger::Level::Debug>("listening to port: ", _port);

        ClientStore::OnRemoveClient([this] (const ClientStore::Client _client) {
            Logger->log<logger::Level::Info>("Removing client from selector (", *_client, ")");
            if (!_client->getSocket()->close())
                Logger->log<logger::Level::Warning>("Closing socket for client failed");
            m_selector.erase(_client->getSocket());
        });
    }

    void TCPInputNetwork::runtime(std::stop_token _st)
    {
        Logger->log<logger::Level::Info>("Launching process unit runtime");

        ClientStore::ClientSocket accept = nullptr;
        std::vector<ClientStore::ClientSocket> clients;

        while (!_st.stop_requested()) {
            accept = m_acceptor.accept();
            if (accept) {
                ClientStore::Instance().newClientSocket(accept);
                m_selector.client(accept);
                Logger->log<logger::Level::Info>("[InNetwork] Accepted new client: "); // todo log
            }
            clients = m_selector.pull();
            if (clients.size())
                Logger->log<logger::Level::Debug>("[InNetwork] Received event from: ", clients.size(), " clients");
            for (ClientStore::ClientSocket &_client : clients) {
                ClientStore::Client client = ClientStore::Instance().findClient(_client);

                if (client == nullptr) {
                    m_selector.erase(_client);
                    (void)_client->close();
                    Logger->log<logger::Level::Warning>("[InNetwork] Unable to find the client's information: ");
                } else if (process(client)) {
                    Logger->log<logger::Level::Info>("[InNetwork] Disconnecting client (", *client, ")");
                    ClientStore::Instance().removeClient(client);
                }
            }
        }
        Logger->log<logger::Level::Info>("Exiting process unit runtime");
    }

    bool TCPInputNetwork::process(const ClientStore::Client &_client)
    {
        int error = 0;

        if (_client->getSocket() == nullptr || !_client->getSocket()->isOpen()) {
            Logger->log<logger::Level::Info>("Client socket not found or closed");
            return true;
        }
        std::vector<std::byte> bytes = _client->getSocket()->receive(2048, error);
        std::string data(reinterpret_cast<const char*>(bytes.data()), + bytes.size());

        Logger->log<logger::Level::Info>("Received from the client: (", _client->getUserId(), "), data:", data);
        if (error == 0) {
            Logger->log<logger::Level::Error>("no data receive from the client: ");
            return true;
        }

        buildHeader(_client, data);
        return false;
    }

    void TCPInputNetwork::buildHeader(const ClientStore::Client &_client, const std::string &_data)
    {
        fix42::Header header{};
        std::unordered_set<uint16_t> tagset{};
        std::vector<std::string> split = utils::split<'\1'>(_data);
        fix::MapMessage map;
        bool header_complet = false;
        uint32_t checksum = 0;

        for (const std::string &result : split) {
            std::vector<std::string> kvsplit = utils::split<'='>(result);

            checksum += CountCheckSum(result);
            if (kvsplit.size() != 2) {
                Logger->log<logger::Level::Error>("No '=' separator found, rejecting message");
                fix42::msg::SessionReject reject{};

                if (tagset.contains(fix42::tag::MsgSeqNum))
                    reject.get<fix42::tag::RefSeqNum>().Value = header.get<fix42::tag::MsgSeqNum>().Value;
                else
                    reject.get<fix42::tag::RefSeqNum>().Value = _client->getSeqNumber();
                if (tagset.contains(fix42::tag::MsgSeqNum))
                reject.get<fix42::tag::RefMsgType>().Value = header.getPositional<fix42::tag::MsgType>().Value;
                else
                    Logger->log<logger::Level::Warning>("Unable to find message type in header, reject has empty RefMsgType");
                reject.get<fix42::tag::SessionRejectReason>().Value = fix42::RejectReasonSession::UndefineTag;
                reject.get<fix42::tag::Text>().Value = "Unable to parse header key-value";
                m_error.append(_client, std::chrono::system_clock::now(), std::move(reject.to_string()));
                return;
            }
            if (!header_complet) {
                Logger->log<logger::Level::Verbose>("Inserting: ", kvsplit[0], " = ", kvsplit[1]);
                xstd::Expected<bool, fix::RejectError> error = header.try_insert(kvsplit[0], kvsplit[1]);

                if (error.has_error()) {
                    Logger->log<logger::Level::Error>("Error during insertion of: ", kvsplit[0], " = ", kvsplit[1], ": ", error.error().Message);
                    m_error.append(_client, std::chrono::system_clock::now(), std::move(BuildRejectFromError(error.error(), tagset, header, _client).to_string()));
                    return;
                }
                if (!error.value()) {
                    std::optional<fix::RejectError> reject = verifyHeader(header, tagset);

                    if (reject.has_value()) {
                        Logger->log<logger::Level::Info>("header verification failed: ", reject.value().Message);
                        m_error.append(_client, std::chrono::system_clock::now(), std::move(BuildRejectFromError(reject.value(), tagset, header, _client).to_string()));
                        // send to output
                        return;
                    }
                    Logger->log<logger::Level::Info>("Header verification completed sucessfully");
                    header_complet = true;
                    continue;
                }
                tagset.emplace(static_cast<uint16_t>(std::stoi(kvsplit[0])));
            } else {
                map.emplace_back(kvsplit[0], kvsplit[1]);
            }
        }
        std::optional<fix::RejectError> reject = verifyCheckSum(checksum, map);

        if (reject.has_value()) {
            Logger->log<logger::Level::Info>("Checksum verification failed: ", reject.value().Message);
            m_error.append(_client, std::chrono::system_clock::now(), std::move(BuildRejectFromError(reject.value(), tagset, header, _client).to_string()));
            return;
        }
        Logger->log<logger::Level::Debug>("Finish parsing the input message");
        m_output.append(_client, std::chrono::system_clock::now(), std::move(header), std::move(map));
    }

    std::optional<fix::RejectError> TCPInputNetwork::verifyHeader(const fix42::Header &_header, const std::unordered_set<uint16_t> &_tagset)
    {
        std::optional<fix::RejectError> error = _header.verify();

        if (error.has_value())
            return error;
        return TCPInputNetwork::HeaderVerifyPresence<fix42::tag::SenderCompId, fix42::tag::TargetCompId, fix42::tag::MsgSeqNum, fix42::tag::SendingTime>(_tagset);
    }

    fix42::msg::SessionReject TCPInputNetwork::BuildRejectFromError(const fix::RejectError &_error, const std::unordered_set<uint16_t> &_tagset, const fix42::Header &_header, const ClientStore::Client &_client)
    {
        fix42::msg::SessionReject reject{};

        if (_tagset.contains(fix42::tag::MsgSeqNum))
            reject.get<fix42::tag::RefSeqNum>().Value = _header.get<fix42::tag::MsgSeqNum>().Value;
        else
            reject.get<fix42::tag::RefSeqNum>().Value = _client->getSeqNumber();
        if (_tagset.contains(fix42::tag::MsgSeqNum))
            reject.get<fix42::tag::RefMsgType>().Value = _header.getPositional<fix42::tag::MsgType>().Value;
        else
            Logger->log<logger::Level::Warning>("Unable to find message type in header, reject has empty RefMsgType");
        reject.get<fix42::tag::SessionRejectReason>().Value = static_cast<fix42::RejectReasonSession>(_error.Reason);
        reject.get<fix42::tag::RefTagId>().Value = _error.Tag;
        reject.get<fix42::tag::Text>().Value = _error.Message;
        return reject;
    }

    uint32_t TCPInputNetwork::CountCheckSum(const std::string &_data)
    {
        uint32_t count = 0;

        for (const char _c : _data)
            count += static_cast<uint32_t>(_c);
        return count + 1;
    }

    std::optional<fix::RejectError> TCPInputNetwork::verifyCheckSum(uint32_t _checksum, fix::MapMessage &_map) const
    {
        uint8_t checksum = static_cast<uint8_t>(_checksum % 256);
        std::optional<fix::RejectError> reject = std::nullopt;
        uint8_t out_checksum = 0;
        std::pair<std::string, std::string> pair ;

        if (_map.size() == 0)
            return fix::RejectError{ fix::RejectError::ReqTagMissing, "Last tag should be CheckSum" };
        pair = _map.back();
        if (pair.first == "10") {
            reject = TagConvertor(pair.second, out_checksum);

            if (reject.has_value())
                return reject;
            if (out_checksum != checksum)
                return fix::RejectError{ fix::RejectError::ValueOORange, "Miss match on checksum" };
            _map.pop_back();
            return std::nullopt;
        } else {
            return fix::RejectError{ fix::RejectError::ReqTagMissing, "Last tag should be CheckSum" };
        }
    }
}