#include "Server/ProcessUnit/Network/TCPInput.hpp"

namespace pu
{
    template<fix::TagName Tag, fix::TagName ...RemainTags>
    std::optional<fix::RejectError> TCPInputNetwork::HeaderVerifyPresence(const std::unordered_set<uint16_t> &_tagset)
    {
        if (!_tagset.contains(Tag))
            return fix::RejectError{ fix::RejectError::ReqTagMissing, "Non positional required tag missing", Tag };
        if constexpr (sizeof...(RemainTags) > 0)
            return HeaderVerifyPresence<RemainTags...>(_tagset);
        else
            return std::nullopt;
    }

}