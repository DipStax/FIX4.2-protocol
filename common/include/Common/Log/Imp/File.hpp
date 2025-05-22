#pragma once

#include <fstream>

#include "Common/Log/Imp/Base.hpp"

namespace log::imp
{
    class File : public Base
    {
        public:
            File(const std::string &_name);
            ~File();

            void newEventLog(Event _event);

        private:
            std::ofstream m_stream;
    };
}