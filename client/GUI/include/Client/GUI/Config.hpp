#pragma once

#include "Shared/Configuration/Configuration.hpp"

namespace config
{
    struct Global
    {
        struct BackendConfig
        {
            std::string Ip{};
        };

        struct InitiatorConfig
        {
            std::string Ip{};
            uint32_t Port = 0;
        };

        BackendConfig Back{};
        InitiatorConfig Initiator{};
        std::string ApiKey;

        friend std::ostream &operator<<(std::ostream &_os, const config::Global &_config);
    };

    void from_json(const nlohmann::json &_json, config::Global &_config);
    void to_json(nlohmann::json &_json, const config::Global &_config);
}

