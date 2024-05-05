#include <fstream>
#include <iostream>

#include "Streamliner/config/Loader.hpp"

namespace sl::config
{
    std::shared_ptr<Node> Loader::run(const std::string &_name, const std::string &_path)
    {
        return declare(_name, run(_path));
    }

    json Loader::run(const std::string &_path)
    {
        std::ifstream file(_path);
        std::cout << _path << std::endl;
        json jconfig;

        file >> jconfig;
        return jconfig;
    }

    std::shared_ptr<Node> Loader::declare(const std::string &_name, const json &_json)
    {
        std::cout << "declaring: " << _name << ": " << _json << "\n";
        m_config[_name] = AsDict(_json);
        return m_config[_name];
    }

    bool Loader::contains(const std::string &_str) const
    {
        return m_config.find(_str) != m_config.end();
    }

    std::shared_ptr<Node> Loader::operator[](const std::string &_name)
    {
        return m_config[_name];
    }

    Loader &Loader::GetInstance()
    {
        static Loader loader{};

        return loader;
    }

    std::shared_ptr<Node> Loader::AsDict(const json &_json)
    {
        std::unordered_map<std::string, std::shared_ptr<Node>> res{};

        for (const auto& [_key, _val] : _json.items()) {
            std::shared_ptr<Node> node;

            if (_val.is_array())
                node = AsArray(_val);
            else if (_val.is_structured())
                node = AsDict(_val);
            else if (_val.is_number_integer())
                node = std::make_shared<Node>(_val.get<int>());
            else if (_val.is_number_float())
                node = std::make_shared<Node>(_val.get<double>());
            else if (_val.is_string())
                node = std::make_shared<Node>(_val.get<std::string>());
            try_include(node, _key, _val);
            res.emplace(_key, node);
        }
        return std::make_shared<Node>(res);
    }

    std::shared_ptr<Node> Loader::AsArray(const json &_json)
    {
        std::vector<std::shared_ptr<Node>> res{};

        for (const auto &_val : _json) {
            std::shared_ptr<Node> node;

            if (_val.is_structured())
                node = AsDict(_val);
            else if (_val.is_number_integer())
                node = std::make_shared<Node>(_val.get<int>());
            else if (_val.is_number_float())
                node = std::make_shared<Node>(_val.get<double>());
            else if (_val.is_string())
                node = std::make_shared<Node>(_val.get<std::string>());
            res.emplace_back(node);
        }
        return std::make_shared<Node>(res);
    }

    std::ostream &operator<<(std::ostream &_os, Loader &_loader)
    {
        _os << "Loader: {\n" << std::setw(4);
        for (const auto &[_key, _val] : _loader.m_config) {
            _os << "- { Name: " << _key << ", Config:\n" << *_val << "} // " << _key << "\n";
        }
        _os << "} // Loader";
        return _os;
    }

    std::shared_ptr<Node> Loader::try_include(std::shared_ptr<Node> _node, std::string _key, json _val)
    {
        Node node = nullptr;

        if (_key != "include")
            return _node;
        if (_val.is_array()) {
            std::vector<std::string> paths = _val.get<std::vector<std::string>>();

            for (const std::string &_path : paths) {
                if (GetInstance().contains(_path))
                    node.merge(GetInstance()[_path]);
                else
                    node.merge(GetInstance().run(_path, _path));
            }
        } else if (_val.is_string()) {
            std::string path = _val.get<std::string>();

            if (GetInstance().contains(path))
                node.merge(GetInstance()[path]);
            else
                node.merge(GetInstance().run(path, path));
        } else {
            return _node;
        }
        return std::make_shared<Node>(node);
    }

}