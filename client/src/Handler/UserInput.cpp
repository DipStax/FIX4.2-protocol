#include <iostream>

#include "Client/Handler/UserInput.hpp"

namespace io
{
    void UserInput::loop()
    {
        while (*this) {
            std::string input;

            std::cout << "$> ";
            std::getline(std::cin, input);
            recv(std::move(input));
            while (!empty(Side::Send))
                (void)pop_front_send();
        }
    }
}