#include "Server/ProcessUnit/data/Global.hpp"

namespace data
{
    template<class T>
    ParsedMessage<T>::ParsedMessage(const fix42::Header &&_header, const T &&_message) noexcept
        : Header(std::move(_header)), Message(std::move(_message))
    {
    }
}