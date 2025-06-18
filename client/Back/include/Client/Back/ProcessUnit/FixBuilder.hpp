#pragma once

#include "Client/Back/ProcessUnit/TransitName.hpp"

#include "Common/Container/IProcessUnitBase.hpp"
#include "Common/Log/ILogger.hpp"
#include "Common/Network/Buffer.hpp"

namespace pu
{
    class FixBuilder : public IProcessUnitBase
    {
        public:
            FixBuilder(ts::Queue<net::Buffer> &_input, QueueMessage &_output);
            virtual ~FixBuilder() = default;

        protected:
            void runtime(std::stop_token _st);

        private:
            template<class T>
            fix::Message build(net::Buffer &_buffer);

            fix::Message buildLogon(net::Buffer &_buffer);
            fix::Message buildOrderSingle(net::Buffer &_buffer);

            ts::Queue<net::Buffer> &m_input;
            QueueMessage &m_output;

            std::unique_ptr<logger::ILogger> Logger = nullptr;
    };
}