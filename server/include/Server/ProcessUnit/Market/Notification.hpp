// #pragma once

// #include "Shared/ProcessUnit/IProcessUnitBase.hpp"
// #include "Server/ProcessUnit/data/Global.hpp"

// #include "Shared/Log/ILogger.hpp"

// #ifndef NOTIF_UPDATE_TO
//     #define NOTIF_UPDATE_TO 5000
// #endif

// namespace pu::market
// {
//     class Notification : public IProcessUnitBase
//     {
//         public:
//             Notification(OrderBook &_ob, InputNetworkOutput &_tcp);
//             virtual ~Notification() = default;

//         protected:
//             void runtime(std::stop_token _st);

//         private:
//             const std::string m_name;

//             InputNetworkOutput &m_tcp_output;
//             OrderBook &m_ob;

//             std::unique_ptr<logger::ILogger> Logger = nullptr;
//     };
// }