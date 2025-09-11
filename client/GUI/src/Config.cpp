#include "Client/GUI/Config.hpp"

namespace config
{
    std::ostream &operator<<(std::ostream &_os, const config::Global &_config)
    {
        nlohmann::json json = _config;

        return _os << "GlobalConfig" << json.dump(2) << std::endl;
    }

    void from_json(const nlohmann::json &_json, config::Global &_config)
    {
        _json.at("api-key").get_to(_config.ApiKey);

        const nlohmann::json &initiator = _json.at("initiator");

        initiator.at("ip").get_to(_config.Initiator.Ip);
        initiator.at("port").get_to(_config.Initiator.Port);

        const nlohmann::json &backend = _json.at("back");

        backend.at("ip").get_to(_config.Back.Ip);
    }

    void to_json(nlohmann::json &_json, const config::Global &_config)
    {
        _json = nlohmann::json{
            {"initiator",  {
                {"ip", _config.Initiator.Ip},
                {"port", _config.Initiator.Port},
            }},
            {"back", {
                {"ip", _config.Back.Ip},
            }},
            {"api-key", _config.ApiKey}
        };
    }
}