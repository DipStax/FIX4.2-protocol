#include <algorithm>
#include <charconv>
#include <iomanip>

#include "Shared/Message-v2/TagConvertor.hpp"

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::RejectReasonSession &_out)
{
    if (!std::all_of(_value.begin(), _value.end(), [] (char _c) { return std::isdigit(_c); }))
        return fix::RejectError{ fix::RejectError::ValueOORange, "Value should be numeric" };
    _out = static_cast<fix42::RejectReasonSession>(std::stoi(_value));
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
        case fix42::OrderStatus::AcceptedBidding_Restated:
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

std::optional<fix::RejectError> TagConvertor(const std::string &_value, std::string &_out)
{
    _out = _value;
    return std::nullopt;
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, char &_out)
{
    if (_value.size() > 1)
        return fix::RejectError{ fix::RejectError::ValueOORange, "Expected a char" };
    _out = _value[0];
    return std::nullopt;
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, uint8_t &_out)
{
    auto [ptr, ec] = std::from_chars(_value.data(), _value.data() + _value.size(), _out);

    if (ec == std::errc() && ptr == _value.data() + _value.size())
        return std::nullopt;
    if (ec == std::errc::invalid_argument)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Expected an uint8_t" };
    else if (ec == std::errc::result_out_of_range)
        return fix::RejectError{ fix::RejectError::ValueOORange, "Value out of range of uint8_t" };
    return fix::RejectError{ fix::RejectError::IncorrectFormat, "Unknow error will parsing uint8_t" };
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, uint16_t &_out)
{
    auto [ptr, ec] = std::from_chars(_value.data(), _value.data() + _value.size(), _out);

    if (ec == std::errc() && ptr == _value.data() + _value.size())
        return std::nullopt;
    if (ec == std::errc::invalid_argument)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Expected an uint16_t" };
    else if (ec == std::errc::result_out_of_range)
        return fix::RejectError{ fix::RejectError::ValueOORange, "Value out of range of uint16_t" };
    return fix::RejectError{ fix::RejectError::IncorrectFormat, "Unknow error will parsing uint16_t" };
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, uint32_t &_out)
{
    auto [ptr, ec] = std::from_chars(_value.data(), _value.data() + _value.size(), _out);

    if (ec == std::errc() && ptr == _value.data() + _value.size())
        return std::nullopt;
    if (ec == std::errc::invalid_argument)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Expected an uint32_t" };
    else if (ec == std::errc::result_out_of_range)
        return fix::RejectError{ fix::RejectError::ValueOORange, "Value out of range of uint32_t" };
    return fix::RejectError{ fix::RejectError::IncorrectFormat, "Unknow error will parsing uint32_t" };
}

std::optional<fix::RejectError> TagConvertor(const std::string &_value, float &_out)
{
    auto [ptr, ec] = std::from_chars(_value.data(), _value.data() + _value.size(), _out);

    if (ec == std::errc() && ptr == _value.data() + _value.size())
        return std::nullopt;
    if (ec == std::errc::invalid_argument)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Expected an float" };
    else if (ec == std::errc::result_out_of_range)
        return fix::RejectError{ fix::RejectError::ValueOORange, "Value out of range of float" };
    return fix::RejectError{ fix::RejectError::IncorrectFormat, "Unknow error will parsing float" };
}


std::optional<fix::RejectError> TagConvertor(const std::string &_value, std::chrono::time_point<std::chrono::system_clock> &_out)
{
    std::tm tm = {};
    auto parse2 = [] (const std::string &_str, size_t _pos, uint8_t &_intout) -> bool {
        if (!std::isdigit(_str[_pos]) || !std::isdigit(_str[_pos+1]))
            return false;
        auto [ptr, ec] = std::from_chars(_str.data() + _pos, _str.data() + _pos + 2, _intout);
        return ec == std::errc();
    };
    auto parse4 = [] (const std::string &_str, size_t _pos, uint16_t &_intout) -> bool {
        for (size_t i = 0; i < 4; ++i)
            if (!std::isdigit(_str[_pos+i])) return false;
        auto [ptr, ec] = std::from_chars(_str.data()+_pos, _str.data()+_pos+4, _intout);
        return ec == std::errc();
    };
    uint16_t year = 0;
    uint8_t month, day, hour, min, sec;

    // std::istringstream stream(_value);

    // stream >> std::get_time(&tm, "%Y%m%d-%H:%M:%S");
    // if (stream.fail())
    //     return fix::RejectError{ fix::RejectError::IncorrectFormat, "Awaited a time format" };

    if (_value.size() != 17 || _value[8] != '-' || _value[11] != ':' || _value[14] != ':')
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Awaited a time format" };

    if (!parse4(_value, 0, year) || !parse2(_value, 4, month) || !parse2(_value, 6, day) || !parse2(_value, 9, hour) || !parse2(_value, 12, min) || !parse2(_value, 15, sec))
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Awaited a time format" };

    tm.tm_year = static_cast<int>(year - 1900);
    tm.tm_mon  = static_cast<int>(month - 1);
    tm.tm_mday = static_cast<int>(day);
    tm.tm_hour = static_cast<int>(hour);
    tm.tm_min  = static_cast<int>(min);
    tm.tm_sec  = static_cast<int>(sec);

    std::time_t time = std::mktime(&tm);

    if (time == -1)
        return fix::RejectError{ fix::RejectError::ValueOORange, "Unknow error on time convertion" };

    _out = std::chrono::system_clock::from_time_t(time);
    return std::nullopt;
}