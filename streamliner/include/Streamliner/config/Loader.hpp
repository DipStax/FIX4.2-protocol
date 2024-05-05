#pragma once

#include "Streamliner/config/Config.hpp"

namespace sl::config
{
    class Loader
    {
        public:
            Loader() = default;
            ~Loader() = default;

            std::shared_ptr<Node> run(const std::string &_name, const std::string &_path);
            [[nodiscard]] json run(const std::string &_path);

            std::shared_ptr<Node> declare(const std::string &_name, const json &_json);

            [[nodiscard]] bool contains(const std::string &_str) const;
            [[nodiscard]] std::shared_ptr<Node> operator[](const std::string &_name);

            [[nodiscard]] static Loader &GetInstance();

            [[nodiscard]] static std::shared_ptr<Node> AsDict(const json &_json);
            [[nodiscard]] static std::shared_ptr<Node> AsArray(const json &_json);

            friend std::ostream &operator<<(std::ostream &_os, Loader &_loader);

        private:
            static std::shared_ptr<Node> try_include(std::shared_ptr<Node> _node, std::string _key, json _val);

            std::unordered_map<std::string, std::shared_ptr<Node>> m_config{};
    };
}