#include "Client/Initiator/Configuration.hpp"

template<class T>
void Configuration<T>::Load(const std::filesystem::path &_path, Configuration<T> &_config)
{
    std::filesystem::path path{_path};

    if (std::filesystem::exists(path))
        std::runtime_error("path not found" + path.string());
    std::cout << "Looking for path: " << path << std::endl;
    std::ifstream file(path.string());
    nlohmann::json json;

    if (!file)
        std::runtime_error("Unable to open the file: " + path.string());
    file >> json;
    _config.Config = json.get<T>();
}