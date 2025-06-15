#pragma once

#include <unordered_map>
#include <string>

#include "Common/Message/Header.hpp"

 // change to '^' for testing or 1 for production
#define FIX_DELIMITER 1

namespace fix
{
    class Message
    {
        public:
            Message() = default;
            virtual ~Message() = default;

            Header header;

            operator std::string();
            [[nodiscard]] std::string to_string();

            [[nodiscard]] static std::size_t getBodyLength(const std::string &_str);
            [[nodiscard]] static std::string getChecksum(const std::string &_str);

            [[nodiscard]] bool contains(const std::string &_key) const;
            [[nodiscard]] const std::string &get(const std::string &_key) const;

            /// @brief Verify if the header receive is correctly formated.
            /// @param _msg Message to check.
            /// @return If the first element is true then second is set, otherwise it rigly formated.
            [[nodiscard]] static std::pair<bool, Reject> VerifyValid(Serializer::AnonMessage &_msg);

        protected:
            friend class Serializer;

            InsertMap<std::string, std::string> m_params{};
    };
}