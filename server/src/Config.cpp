#include "Server/Config.hpp"

namespace config
{
    std::ostream &operator<<(std::ostream &_os, const config::Global &_config)
    {
        nlohmann::json json = _config;

        return _os << "GlobalConfig" << json.dump(2) << std::endl;
    }

    void from_json(const nlohmann::json &_json, config::Global &_config)
    {
        const nlohmann::json &user = _json.at("user");
        const nlohmann::json hb = user.at("heartbeat");

        hb.at("max-to").get_to(_config.User.Heartbeat.MaxTO);

        const nlohmann::json &network = _json.at("network");

        network.at("tcp-port").get_to(_config.Network.TcpPort);

        const nlohmann::json &market = _json.at("market");

        market.at("list").get_to(_config.Market);
    }

    void to_json(nlohmann::json &_json, const config::Global &_config)
    {
        _json = nlohmann::json{
            {"user", {
                {"heartbeat", {
                    {"max-to", _config.User.Heartbeat.MaxTO}
                }}
            }},
            {"network", {
                {"tcp-port", _config.Network.TcpPort}
            }},
            {"market", {
                {"list", _config.Market}
            }}
        };
    }
}
