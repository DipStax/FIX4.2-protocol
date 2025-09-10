#pragma once

#include "Shared/Configuration/Configuration.hpp"

namespace config
{
    struct Global
    {
        struct UserConfig
        {
            struct HearthBeatConfig
            {
                float MaxTO = 0.f;
            };

            HearthBeatConfig Heartbeat{};
        };

        struct NetworkConfig
        {
            uint32_t TcpPort = 0;
        };

        UserConfig User{};
        NetworkConfig Network{};
        std::vector<std::string> Market;

        friend std::ostream &operator<<(std::ostream &_os, const config::Global &_config);
    };

    void from_json(const nlohmann::json &_json, config::Global &_config);
    void to_json(nlohmann::json &_json, const config::Global &_config);
}

