#include "Client/Back/Config.hpp"

namespace config
{
    std::ostream &operator<<(std::ostream &_os, const config::Global &_config)
    {
        nlohmann::json json = _config;

        return _os << "GlobalConfig" << json.dump(2) << std::endl;
    }

    void from_json(const nlohmann::json &_json, config::Global &_config)
    {
        const nlohmann::json &server = _json.at("fix-server");

        server.at("ip").get_to(_config.FixServer.Ip);
        server.at("port").get_to(_config.FixServer.Port);
        server.at("provider").get_to(_config.FixServer.ProviderName);
    }

    void to_json(nlohmann::json &_json, const config::Global &_config)
    {
        _json = nlohmann::json{
            {"fix-server",  {
                {"ip", _config.FixServer.Ip},
                {"port", _config.FixServer.Port},
                {"provider", _config.FixServer.ProviderName},
            }},
        };
    }
}