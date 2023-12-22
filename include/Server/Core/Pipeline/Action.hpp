#pragma once

#include "Server/Core/Pipeline/Core.hpp"
#include "Server/Core/Pipeline/Naming.hpp"

namespace pip
{
    class Action : public Pipeline<Action>
    {
        public:

            using PipeType = Pipeline<Action>;

            Action(NetToSerial &_input, SerialToMarket &_output, RawOutput &_raw);
            ~Action();

            [[nodiscard]] bool start();
            bool status(float _to = 1.f);

            void loop();

        private:
            NetToSerial &m_input;
            SerialToMarket &m_output;
            RawOutput &m_raw;
    };
}