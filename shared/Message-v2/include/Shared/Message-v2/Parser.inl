#include "Shared/Message-v2/Parser.hpp"

namespace fix42
{
    template<class T>
    xstd::Expected<T, fix42::msg::SessionReject> parseMessage(const fix::MapMessage &_msg, const fix42::Header &_header)
    {
        T testreq{};
        std::optional<fix::RejectError> error = testreq.from_string(_msg);

        if (error.has_value()) {
            fix42::msg::SessionReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = T::Type;
            reject.get<fix42::tag::SessionRejectReason>().Value = static_cast<fix42::RejectReasonSession>(error.value().Reason);
            reject.get<fix42::tag::RefTagId>().Value = error.value().Tag;
            reject.get<fix42::tag::Text>().Value = error.value().Message;
            return xstd::Unexpected<fix42::msg::SessionReject>(std::move(reject));
        }
        return testreq;
    }
}