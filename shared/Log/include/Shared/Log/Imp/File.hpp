#pragma once

#include <fstream>
#include <mutex>

#include "Shared/Log/Imp/Base.hpp"

namespace logger::imp
{
    class File : public Base
    {
        public:
            File(const std::string &_name);
            ~File();

            static void Init(const std::string _file);
            static void Deinit();

            void newEventLog(Event _event);

        private:
            static void moveFile(const std::filesystem::path &_file, size_t _it = 0);

            inline static std::mutex m_mutex{};

            inline static std::ofstream m_stream{};
    };
}