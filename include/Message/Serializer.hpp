#pragma once

#include "Message/Fix.hpp"

namespace fix
{
    class Serializer
    {
        public:
            using AnonMessage = InsertMap<std::string, std::string>;
            using Token = InsertMap<std::string, std::string>::Pair;

            enum class Error
            {
                None,
                InvalidEnd,
                NoEqual,
                InvalidKey
            };

            static Error run(std::string &_msg, AnonMessage &_map);
            static Error token(std::string &_msg, Token &_pair);
    };
}