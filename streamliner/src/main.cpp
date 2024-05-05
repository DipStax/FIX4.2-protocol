#include <iostream>

#include "Streamliner/config/Loader.hpp"

int main()
{
    sl::config::Loader loader{};

    std::cout << "FIRST " << loader << "\n";

    json jconfig =  loader.run("../config/server.json");
    std::shared_ptr<sl::config::Node> config = loader.declare("my config", jconfig);

    std::cout << "SECOND " << loader << "\n";

    std::cout << config << "over\n";
}