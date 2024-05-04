#include <fstream>
#include <iostream>

#include "Streamliner/config/ConfigLoader.hpp"

namespace sl::config
{
    void Loader::run(const std::string &_name, const std::string &_path)
    {
        declare(_name, run(_path));
    }

    json Loader::run(const std::string &_path)
    {
        std::ifstream file(_path);
        json jconfig;

        file >> jconfig;
        return jconfig;
    }

    Node Loader::declare(const std::string &_name, const json &_json)
    {
        m_config[_name] = _json;
        return AsConfig(_json);
    }

    const json &Loader::getJson(const std::string &_name) const
    {
        return m_config.at(_name);
    }

    json &Loader::getJson(const std::string &_name)
    {
        return m_config.at(_name);
    }

    Node Loader::operator[](const std::string &_name)
    {
        return AsConfig(m_config[_name]);
    }

    Node Loader::AsConfig(const json &_json)
    {
        return AsDict(_json);
    }

    Node Loader::AsDict(const json &_json)
    {
        std::unordered_map<std::string, Node> res{};

        for (const auto& [_key, _val] : _json.items()) {
            Node node;

            if (_val.is_array())
                node = AsArray(_val);
            else if (_val.is_structured())
                node = AsDict(_val);
            else if (_val.is_number_integer())
                node = _val.get<int>();
            else if (_val.is_number_float())
                node = _val.get<double>();
            else if (_val.is_string())
                node = _val.get<std::string>();
            res.emplace(_key, node);
        }
        return Node::MakeDict(res);
    }

    Node Loader::AsArray(const json &_json)
    {
        std::vector<Node> res{};

        for (const auto &_val : _json) {
            Node node;

            if (_val.is_structured())
                node = AsDict(_val);
            else if (_val.is_number_integer())
                node = _val.get<int>();
            else if (_val.is_number_float())
                node = _val.get<double>();
            else if (_val.is_string())
                node = _val.get<std::string>();
            res.emplace_back(node);
        }
        return Node::MakeArray(res);
    }

    std::ostream &operator<<(std::ostream &_os, Loader &_loader)
    {
        _os << "Loader: {\n" << std::setw(4);
        for (const auto &[_key, _val] : _loader.m_config)
            _os << "- { Name: " << _key << ", Config:\n" << _val << "} // " << _key << "\n";
        _os << "} // Loader";
        return _os;
    }
}