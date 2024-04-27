#pragma once

#include <concepts>
#include <unordered_map>
#include <fstream>
#include <string>
#include <thread>

template<class T>
concept ToString = requires (T _val) {
    { std::to_string(_val) } -> std::convertible_to<std::string>;
};

#ifndef LOG_DFT_FILE
    #define LOG_DFT_FILE "./default.log"
#endif

class Logger
{
    public:
        static void Init(const std::string &_path = LOG_DFT_FILE);

        static void SetThreadName(std::thread::id _id, const std::string &_name);

        template<class ...Ts>
        static void Log(Ts &&..._args);

    private:
        template<class ...Ts>
        static void LogImpl(std::ostream &_os, Ts &&..._val);

        inline static std::ofstream m_file;
        inline static std::unordered_map<std::thread::id, std::string> m_thread_name;
};

#include "Common/Core/Logger.inl"