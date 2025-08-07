#include "Client/Initiator/Config.hpp"

namespace config
{
    void from_json(const nlohmann::json &_json, config::Global &_config)
    {
        nlohmann::json front = _json.at("front");
        nlohmann::json back = _json.at("back");

        front.at("port").get_to(_config.Front.Port);
        back.at("exec-path").get_to(_config.Back.Executable);
        back.at("api-key").get_to(_config.Back.ApiKey);
        back.at("address").get_to(_config.Back.Address);
    }

    void to_json(nlohmann::json &_json, const config::Global &_config)
    {
        _json = nlohmann::json{
            {"front", {{"port", _config.Front.Port}}},
            {"back",  {
                {"exec-path", _config.Back.Executable},
                {"api-key", _config.Back.ApiKey},
                {"address", _config.Back.Address}
            }},
        };
    }
}

std::ostream &operator<<(std::ostream &_os, const config::Global &_config)
{
    nlohmann::json json = _config;

    return _os << "GlobalConfig" << json.dump(2) << std::endl;
}