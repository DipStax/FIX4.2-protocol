#pragma once

#include "Shared/Configuration/Configuration.hpp"

namespace config
{
    struct Global
    {
        struct BackendConfig
        {
            std::string Executable{};
            std::vector<std::string> ExecArgs{};
            std::string Address{};
        };

        struct FrontendConfig
        {
            uint32_t Port = 0;
        };

        struct DataBaseConfig
        {
            std::string ConnectionString{};
        };

        FrontendConfig Front{};
        BackendConfig Back{};
        DataBaseConfig DBConfig{};

        friend std::ostream &operator<<(std::ostream &_os, const config::Global &_config);
    };

    void from_json(const nlohmann::json &_json, config::Global &_config);
    void to_json(nlohmann::json &_json, const config::Global &_config);
}

