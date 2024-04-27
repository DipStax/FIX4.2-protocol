#include <algorithm>

#include "Common/Message/Serializer.hpp"
#include <iostream>

namespace fix
{
    Serializer::Error Serializer::run(std::string &_msg, AnonMessage &_map)
    {
        Token pair;

        _map.clear();
        while (!_msg.empty()) {
            Error err = token(_msg, pair);
            if (err != Error::None)
                return err;
            _map.emplace(std::move(pair));
        }
        return Error::None;
    }

    Serializer::Error Serializer::token(std::string &_msg, Token &_pair)
    {
        size_t size = _msg.find((char)FIX_DELIMITER);

        if (size == std::string::npos)
            return Error::InvalidEnd;
        size_t split = _msg.find("=");

        if (split >= size)
            return Error::NoEqual;
        std::string key = _msg.substr(0, split);

        if (key.empty() && std::all_of(key.begin(), key.end(), ::isdigit))
            return Error::InvalidKey;
        _pair = { std::move(key), std::move(_msg.substr(split + 1, size - (key.size() + 1))) };
        _msg.erase(0, size + 1);
        return Error::None;
    }

    std::string Serializer::serialize(const AnonMessage &_map)
    {
        std::string msg;

        for (const auto &[key, val] : _map)
            msg += key + "=" + val + (char)FIX_DELIMITER;
        return msg;
    }
}