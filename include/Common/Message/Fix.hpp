#pragma once

#include <unordered_map>
#include <string>

#include "Common/Message/Header.hpp"
#include "Common/Container/InsertMap.hpp"

#define FIX_DELIMITER 1 // change to '^' for testing or 1 for production

namespace fix
{
    class Message
    {
        public:
            Message() = default;
            virtual ~Message() = default;

            Header header;

            operator std::string ();

            std::string to_string();

        protected:
            friend class Serializer;

            InsertMap<std::string, std::string> m_params{};
    };
}