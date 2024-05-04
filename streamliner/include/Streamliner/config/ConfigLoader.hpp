#pragma once

#include <nlohmann/json.hpp>

#include "Streamliner/config/Config.hpp"

using json = nlohmann::json;

namespace sl::config
{
    class Loader
    {
        public:
            Loader() = default;
            ~Loader() = default;

            void run(const std::string &_name, const std::string &_path);
            json run(const std::string &_path);

            Node declare(const std::string &_name, const json &_json);

            const json &getJson(const std::string &_name) const;
            json &getJson(const std::string &_name);

            Node operator[](const std::string &_name);

            static Node AsConfig(const json &_json);

            friend std::ostream &operator<<(std::ostream &_os, Loader &_loader);

        private:
            static Node AsDict(const json &_json);
            static Node AsArray(const json &_json);

            std::map<std::string, json> m_config{};
    };
}