#pragma once

#include <fstream>
#include <mutex>

#include "Common/Log/Imp/Base.hpp"

namespace logger::imp
{
    class File : public Base
    {
        public:
            File(const std::string &_name);
            ~File();

            void newEventLog(Event _event);

        private:
            std::mutex m_mutex;

            std::ofstream m_stream;
    };
}