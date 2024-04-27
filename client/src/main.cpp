#include "Client/Core.hpp"

int main(int _ac, const char **_av)
{
    std::ignore = _ac;
    std::ignore = _av;
    Core core("127.0.0.1", 8080, 8081);

    core.start();
    return 0;
}