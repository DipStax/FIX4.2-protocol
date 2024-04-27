#pragma once

#ifndef FIX_DELIMITER
    #define FIX_DELIMITER 1
#endif

#include <string>

#include "Common/Container/InsertMap.hpp"

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

            // REMOVE IF NOT USED LATER
            static std::string serialize(const AnonMessage &_map);
    };
}