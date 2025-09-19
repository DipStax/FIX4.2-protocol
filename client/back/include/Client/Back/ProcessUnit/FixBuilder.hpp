#pragma once

#include "Client/Back/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AProcessUnitBase.hpp"
#include "Shared/Log/ILogger.hpp"
#include "Shared/Network/Buffer.hpp"

namespace pu
{
    class FixBuilder : public AProcessUnitBase
    {
        public:
            FixBuilder(ts::Queue<net::Buffer> &_input, StringOutputQueue &_output);
            virtual ~FixBuilder() = default;

        protected:
            void runtime(std::stop_token _st) final;

        private:

            data::StringOutput buildLogon(net::Buffer &_buffer);
            data::StringOutput buildOrderSingle(net::Buffer &_buffer);

            ts::Queue<net::Buffer> &m_input;
            StringOutputQueue &m_output;
    };
}