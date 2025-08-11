#pragma once

#include <nlohmann/json.hpp>

namespace config
{
    struct Global
    {
        struct BackendConfig
        {
            std::string Executable{};
            std::string ApiKey{};
            std::string Address{};
        };

        struct FrontendConfig
        {
            uint32_t Port = 0;
        };

        FrontendConfig Front;
        BackendConfig Back;
    };

    void from_json(const nlohmann::json &_json, config::Global &_config);
    void to_json(nlohmann::json &_json, const config::Global &_config);
}


std::ostream &operator<<(std::ostream &_os, const config::Global &_config);