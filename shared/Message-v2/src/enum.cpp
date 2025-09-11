#include "Shared/Message-v2/enum.hpp"

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::RejectReasonSession &_out)
{
    try {
        _out = static_cast<fix42::RejectReasonSession>(std::stoi(_value));
    } catch (const std::exception &_exception) {
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Invalid format expected a char" };
    }
    switch (_out) {
        case fix42::RejectReasonSession::InvalidTagNum:
        case fix42::RejectReasonSession::RequiredTagMissing:
        case fix42::RejectReasonSession::TagNotDefine:
        case fix42::RejectReasonSession::UndefineTag:
        case fix42::RejectReasonSession::TagWithoutValue:
        case fix42::RejectReasonSession::ValueOutOfRange:
        case fix42::RejectReasonSession::IncorrectDataFormat:
        case fix42::RejectReasonSession::SignatureProblem:
        case fix42::RejectReasonSession::CompIDProblem:
        case fix42::RejectReasonSession::SendingTimeAccuracy:
        case fix42::RejectReasonSession::InvalidMsgType:
            return std::nullopt;
        default:
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 1 <= value <= 10" };
    }
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::TransactionType &_out)
{
    if (_value.size() > 1)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Value to long" };
    _out = static_cast<fix42::TransactionType>(_value[0]);
    switch (_out) {
        case fix42::TransactionType::New:
        case fix42::TransactionType::Cancel:
        case fix42::TransactionType::Correct:
        case fix42::TransactionType::Status:
            return std::nullopt;
        default:
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 0 <= value <= 3" };
    }
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::OrderStatus &_out)
{
    if (_value.size() > 1)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Value to long" };
    _out = static_cast<fix42::OrderStatus>(_value[0]);
    switch (_out) {
        case fix42::OrderStatus::NewOrder:
        case fix42::OrderStatus::PartiallyFilled:
        case fix42::OrderStatus::Filled:
        case fix42::OrderStatus::DoneForDay:
        case fix42::OrderStatus::Canceled:
        case fix42::OrderStatus::Replaced:
        case fix42::OrderStatus::PedningCancel:
        case fix42::OrderStatus::Stopped:
        case fix42::OrderStatus::Rejected:
        case fix42::OrderStatus::Suspended:
        case fix42::OrderStatus::PendingNew:
        case fix42::OrderStatus::Caluclated:
        case fix42::OrderStatus::Expired:
        case fix42::OrderStatus::AcceptedBidding:
        case fix42::OrderStatus::PendingReplace:
            return std::nullopt;
        default:
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 0 <= value <= 9 || A <= value <= E" };
    }
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::Side &_out)
{
    if (_value.size() > 1)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Value to long" };
    _out = static_cast<fix42::Side>(_value[0]);
    switch (_out) {
        case fix42::Side::Buy:
        case fix42::Side::Sell:
        case fix42::Side::BuyMinus:
        case fix42::Side::SellPlus:
        case fix42::Side::SellShort:
        case fix42::Side::SellShortExempt:
        case fix42::Side::Undisclosed:
        case fix42::Side::Cross:
        case fix42::Side::CrossShort:
            return std::nullopt;
        default:
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 1 <= value <= 9" };
    };
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::RejectReasonBusiness &_out)
{
    if (_value.size() > 1)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Value to long" };
    _out = static_cast<fix42::RejectReasonBusiness>(_value[0]);
    switch (_out) {
        case fix42::RejectReasonBusiness::Other:
        case fix42::RejectReasonBusiness::UnknownId:
        case fix42::RejectReasonBusiness::UnknownSecurity:
        case fix42::RejectReasonBusiness::UnsupportedMsgType:
        case fix42::RejectReasonBusiness::AppNotAvailable:
        case fix42::RejectReasonBusiness::CondReqFieldMissing:
            return std::nullopt;
        default:
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 0 <= value <= 5" };
    }
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::EncryptionMethod &_out)
{
    if (_value.size() > 1)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Value to long" };
    _out = static_cast<fix42::EncryptionMethod>(_value[0]);
    switch (_out) {
        case fix42::EncryptionMethod::None:
        case fix42::EncryptionMethod::PKCS:
        case fix42::EncryptionMethod::DES:
        case fix42::EncryptionMethod::PKCSDES:
        case fix42::EncryptionMethod::PGPDES:
        case fix42::EncryptionMethod::PGPDESMD5:
        case fix42::EncryptionMethod::PEMDESMD5:
            return std::nullopt;
        default:
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 0 <= value <= 6" };
    }
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::HandleInstance &_out)
{
    if (_value.size() > 1)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Value to long" };
    _out = static_cast<fix42::HandleInstance>(_value[0]);
    switch (_out) {
        case fix42::HandleInstance::AutoPrivate:
        case fix42::HandleInstance::AutoPublic:
        case fix42::HandleInstance::Manual:
            return std::nullopt;
        default:
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 1 <= value <= 3" };
    }
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::OrderType &_out)
{
    if (_value.size() > 1)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Value to long" };
    _out = static_cast<fix42::OrderType>(_value[0]);
    switch (_out) {
        case fix42::OrderType::Market:
        case fix42::OrderType::Limit:
        case fix42::OrderType::Stop:
        case fix42::OrderType::StopLimit:
        case fix42::OrderType::MarketOnClose:
        case fix42::OrderType::WithOrWithout:
        case fix42::OrderType::LimitOrBetter:
        case fix42::OrderType::LimitWithOrWithout:
        case fix42::OrderType::OnBasis:
        case fix42::OrderType::OnClose:
        case fix42::OrderType::LimitOnClose:
        case fix42::OrderType::ForexMarket:
        case fix42::OrderType::PrevQuoted:
        case fix42::OrderType::PrevIndicated:
        case fix42::OrderType::ForexLimit:
        case fix42::OrderType::ForexSwap:
        case fix42::OrderType::ForexPrevQuoted:
        case fix42::OrderType::Funari:
        case fix42::OrderType::Pegged:
            return std::nullopt;
        default:
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 1 <= value <= 9 || A <= value <= H || value == P" };
    }
}
