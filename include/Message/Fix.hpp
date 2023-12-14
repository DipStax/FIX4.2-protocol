#pragma once

#include <unordered_map>
#include <string>

#include "Message/Header.hpp"
#include "Container/InsertMap.hpp"

#define FIX_DELIMITER '^' // change to 1 for real FIX protocol

namespace fix
{
    class Message
    {
        public:
            Message() = default;
            virtual ~Message() = default;

            Header header;

            // operator std::string () const;
            operator std::string ();

        protected:
            friend class Serializer;

            InsertMap<std::string, std::string> m_params{};
    };
}