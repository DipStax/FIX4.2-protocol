#pragma once

#include "Shared/Configuration/Configuration.hpp"

namespace config
{
    struct Global
    {
        struct FixServerConfig
        {
            std::string Ip{};
            uint32_t Port = 0;
            std::string ProviderName{};
        };

        FixServerConfig FixServer{};

        friend std::ostream &operator<<(std::ostream &_os, const config::Global &_config);
    };

    void from_json(const nlohmann::json &_json, config::Global &_config);
    void to_json(nlohmann::json &_json, const config::Global &_config);
}

