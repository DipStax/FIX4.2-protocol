#include "Common/Core/Logger.hpp"
#include "Server/Network/Processor.hpp"

#include "Common/Message/Serializer.hpp"
#include "Common/Message/Reject.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

namespace net::tcp
{
    namespace in
    {
        bool Basic::run(ClientStore::Client _client, InputRouter &_serial, InOutNetwork &_error)
        {
            int error = 0;
            fix::Serializer::AnonMessage msg;
            fix::Reject reject;

            if (!_client->getSocket()) { // todo is_open
                Logger::Log("[Processor] Client disconnected");
                return true;
            }
            std::string data(_client->getSocket()->receive(MAX_RECV_SIZE, error));

            Logger::Log("[Processor] Received from the client: (", _client->User, "), data:", data);
            if (error == 0) {
                Logger::Log("[Processor] Error: no data receive from the client: ");
                return true;
            }
            if (fix::Serializer::run(data, msg) != fix::Serializer::Error::None) {
                Logger::Log("[Processor] Error: will parsing the client message");
                // build reject
                _error.append(_client, std::move(reject));
                return false;
            }
            _client->newRequest();
            _serial.append(_client, std::move(msg));
            return false;
        }
    }

    namespace out
    {
        namespace priv
        {
            void LogTiming(ClientStore::Client _client)
            {
                if (!_client->hasRequest(_client->SeqNumber - 1))
                    return;
                auto start = _client->getRequest(_client->SeqNumber - 1);
                auto end = std::chrono::system_clock::now();
                auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                Logger::Log("[Responce] Send respond after: ", diff.count(), " ms");
            }
        }

        bool Response::run(OutNetworkInput &_data)
        {
            Logger::Log("here");
            _data.Message.header.set49_SenderCompId(PROVIDER_NAME);
            _data.Message.header.set34_msgSeqNum(std::to_string((_data.Client->SeqNumber)++));
            _data.Message.header.set56_TargetCompId(_data.Client->User);
            std::string data = _data.Message.to_string();

            Logger::Log("here", _data.Client->getSocket() == nullptr);
            if (_data.Client->getSocket()) { // todo is_open
                Logger::Log("here");
                if (_data.Client->getSocket()->send(reinterpret_cast<const uint8_t *>(data.c_str()), data.size()) == data.size())
                    Logger::Log("[Responce] Data send successfuly: ", data);
                else
                    Logger::Log("[Responce] Error occured when sending data");
                if (!_data.Client->Logged)
                priv::LogTiming(_data.Client);
                Logger::Log("[Responce] Updated client status: { UserId: ", _data.Client->User, " }"); // todo log
                if (_data.Client->Disconnect) {
                    _data.Client->getSocket()->close();
                    Logger::Log("[Responce] Client has been disconnected: ", _data.Client->User);
                    ClientStore::Instance().removeClient(_data.Client);
                }
            } else {
                Logger::Log("[Responce] Client not connected: ", _data.Client->User);
                ClientStore::Instance().removeClient(_data.Client);
                return true;
            }
            return false;
        }

        bool SubResponse::run(OutNetworkInput &_data)
        {
            std::string data{};

            _data.Message.header.set49_SenderCompId(PROVIDER_NAME);
            _data.Message.header.set34_msgSeqNum(std::to_string((_data.Client->SeqNumber)++));
            _data.Message.header.set56_TargetCompId(_data.Client->User);
            data = _data.Message.to_string();
            if (_data.Client->getSocket()->send(reinterpret_cast<const uint8_t *>(data.c_str()), data.size()) == data.size())
                Logger::Log("[Responce] Data send successfuly: ", data);
            else
                Logger::Log("[Responce] Error occured when sending data");
            priv::LogTiming(_data.Client);
            return false;
        }
    }
}