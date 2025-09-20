#include "Client/Initiator/Config.hpp"

namespace config
{
    std::ostream &operator<<(std::ostream &_os, const config::Global &_config)
    {
        nlohmann::json json = _config;

        return _os << "GlobalConfig" << json.dump(2) << std::endl;
    }

    void from_json(const nlohmann::json &_json, config::Global &_config)
    {
        const nlohmann::json &front = _json.at("front");

        front.at("port").get_to(_config.Front.Port);

        const nlohmann::json &back = _json.at("back");

        back.at("exec-path").get_to(_config.Back.Executable);
        back.at("args").get_to(_config.Back.ExecArgs);
        back.at("address").get_to(_config.Back.Address);

        const nlohmann::json &db = _json.at("database");

        db.at("connstr").get_to(_config.DBConfig.ConnectionString);
    }

    void to_json(nlohmann::json &_json, const config::Global &_config)
    {
        _json = nlohmann::json{
            {"front", {{"port", _config.Front.Port}}},
            {"back",  {
                {"exec-path", _config.Back.Executable},
                {"args", _config.Back.ExecArgs},
                {"address", _config.Back.Address}
            }},
            {"database", {
                {"connstr", _config.DBConfig.ConnectionString}
            }}
        };
    }
}
