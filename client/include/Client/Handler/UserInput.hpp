#pragma once

#include "Client/Handler/Entry.hpp"
#include "Client/Handler/Handler.hpp"
#include "Common/Thread/Queue.hpp"

namespace io
{
    class UserInput : public Handler<Entry, std::string>
    {
        public:
            UserInput() = default;
            ~UserInput() = default;

        private:
            virtual void loop() override;

            std::string m_input;
    };
}