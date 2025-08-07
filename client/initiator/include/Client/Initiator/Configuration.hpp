#pragma once

#include <filesystem>

template<class T>
class Configuration
{
    public:
        ~Configuration() = default;

        T Config{};

        static void Load(const std::filesystem::path &_path, Configuration<T> &_config);

        static Configuration<T> &Get();

    private:
        Configuration() = default;
};

#include "Client/Initiator/Configuration.hpp"