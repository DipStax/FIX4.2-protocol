#include <iostream>

#include "Streamliner/config/ConfigLoader.hpp"

int main()
{
    sl::config::Loader loader{};

    std::cout << loader << "\n";

    json jconfig =  loader.run("config.json");

    std::cout << loader << "\n";

    sl::config::Node config = loader.declare("my config", jconfig);

    std::cout << loader << "\n";

    std::cout << config << "over\n";
}