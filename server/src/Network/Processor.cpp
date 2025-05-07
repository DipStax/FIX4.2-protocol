#include "Server/Network/Processor.hpp"

#include "Common/Core/Logger.hpp"
#include "Common/Message/Serializer.hpp"
#include "Common/Message/Reject.hpp"

namespace net::tcp
{
    namespace in
    {
        bool Basic::run(const ClientStore::Client &_client, InputRouter &_serial, InputNetworkOutput &_error)
        {
            int error = 0;
            fix::Serializer::AnonMessage msg;
            fix::Reject reject;

            if (!_client->getSocket()) { // todo is_open
                Logger::Log("[Processor] Client disconnected");
                return true;
            }
            std::string data(_client->getSocket()->receive(MAX_RECV_SIZE, error));

            Logger::Log("[Processor] Received from the client: (", _client->getUserId(), "), data:", data);
            if (error == 0) {
                Logger::Log("[Processor] Error: no data receive from the client: ");
                return true;
            }
            if (fix::Serializer::run(data, msg) != fix::Serializer::Error::None) {
                Logger::Log("[Processor] Error: will parsing the client message");
                // build reject
                _error.append(_client, std::chrono::system_clock::now(), std::move(reject));
                return false;
            }
            _serial.append(_client, std::chrono::system_clock::now(), std::move(msg));
            return false;
        }
    }

    namespace out
    {
        bool Response::run(Context<data::OutNetworkInput> &_data)
        {
            _data.Message.header.set34_msgSeqNum(std::to_string(_data.Client->nextSeqNumber()));
            _data.Message.header.set49_SenderCompId(PROVIDER_NAME);
            _data.Message.header.set56_TargetCompId(_data.Client->getUserId());

            std::string data = _data.Message.to_string();

            if (_data.Client->getSocket()) { // todo is_open
                if (_data.Client->getSocket()->send(reinterpret_cast<const uint8_t *>(data.c_str()), data.size()) == data.size())
                    Logger::Log("[Responce] Data send successfuly: ", data);
                else
                    Logger::Log("[Responce] Error occured when sending data");
                // todo log timing
                Logger::Log("[Responce] Updated client status: { UserId: ", _data.Client->getUserId(), " }"); // todo log
                if (_data.Client->shouldDisconnect()) {
                    _data.Client->disconnect();
                    Logger::Log("[Responce] Client has been disconnected: ", _data.Client->getUserId());
                }
            } else {
                Logger::Log("[Responce] Client not connected: ", _data.Client->getUserId());
                ClientStore::Instance().removeClient(_data.Client);
                return true;
            }
            return false;
        }
    }
}