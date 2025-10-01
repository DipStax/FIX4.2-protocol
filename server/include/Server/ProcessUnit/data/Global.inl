#include "Server/ProcessUnit/data/Global.hpp"

namespace data
{
    template<class T>
    ParsedMessage<T>::ParsedMessage(const ExecId &&_execId, const fix42::Header &&_header, const T &&_message) noexcept
        : ExecutionId(std::move(_execId)), Header(std::move(_header)), Message(std::move(_message))
    {
    }
}