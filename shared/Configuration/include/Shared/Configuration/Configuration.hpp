#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>

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

#include "Shared/Configuration/Configuration.inl"