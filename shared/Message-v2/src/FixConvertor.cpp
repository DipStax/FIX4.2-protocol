#include <algorithm>
#include <charconv>
#include <iomanip>

#include "Shared/Message-v2/FixConvertor.hpp"
#include "FIX-Message/meta/limit.hpp"

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::RejectReasonSession &_out)
{
    uint8_t out;

    auto [ptr, ec] = std::from_chars(_value.data(), _value.data() + _value.size(), out);

    if (ec == std::errc() && ptr == _value.data() + _value.size()) {
        _out = static_cast<fix42::RejectReasonSession>(out);
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
                return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 1 GE value GE 10" };
        }
    } else if (ec == std::errc::invalid_argument) {
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Expected an uint8_t" };
    } else if (ec == std::errc::result_out_of_range) {
        return fix::RejectError{ fix::RejectError::ValueOORange, "Value out of range of uint8_t" };
    }
    return fix::RejectError{ fix::RejectError::IncorrectFormat, "Unknow error will parsing uint8_t" };
}

void to_FIX(std::string &_out, const fix42::RejectReasonSession _value)
{
    char buffer[2];
    auto [ptr, _] = std::to_chars(buffer, buffer + sizeof(buffer), _value);

    _out.append(buffer, ptr);
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::TransactionType &_out)
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
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 0 GE value GE 3" };
    }
}

void to_FIX(std::string &_out, const fix42::TransactionType _value)
{
    _out.push_back(static_cast<char>(_value));
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::OrderStatus &_out)
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
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 0 GE value GE 9 || A GE value GE E" };
    }
}

void to_FIX(std::string &_out, const fix42::OrderStatus _value)
{
    _out.push_back(static_cast<char>(_value));
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::OrderRejectReason &_out)
{
    if (_value.size() > 1)
        return fix::RejectError{ fix::RejectError::IncorrectFormat, "Value to long" };
    _out = static_cast<fix42::OrderRejectReason>(_value[0]);
    switch (_out) {
        case fix42::OrderRejectReason::BrokerOption:
        case fix42::OrderRejectReason::UnknowSymbol:
        case fix42::OrderRejectReason::ExchangeClose:
        case fix42::OrderRejectReason::OrderExceedLimit:
        case fix42::OrderRejectReason::TooLate:
        case fix42::OrderRejectReason::UnknownOrder:
        case fix42::OrderRejectReason::Duplicate:
        case fix42::OrderRejectReason::DuplicateVerbally:
        case fix42::OrderRejectReason::StaleOrder:
            return std::nullopt;
        default:
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 0 GE value GE 8" };
    }
}
void to_FIX(std::string &_out, const fix42::OrderRejectReason _value)
{
    _out.push_back(static_cast<char>(_value));
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::Side &_out)
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
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 1 GE value GE 9" };
    };
}

void to_FIX(std::string &_out, const fix42::Side _value)
{
    _out.push_back(static_cast<char>(_value));
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::RejectReasonBusiness &_out)
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
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 0 GE value GE 5" };
    }
}

void to_FIX(std::string &_out, const fix42::RejectReasonBusiness _value)
{
    _out.push_back(static_cast<char>(_value));
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::EncryptionMethod &_out)
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
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 0 GE value GE 6" };
    }
}

void to_FIX(std::string &_out, const fix42::EncryptionMethod _value)
{
    _out.push_back(static_cast<char>(_value));
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::HandleInstance &_out)
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
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 1 GE value GE 3" };
    }
}

void to_FIX(std::string &_out, const fix42::HandleInstance _value)
{
    _out.push_back(static_cast<char>(_value));
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::OrderType &_out)
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
            return fix::RejectError{ fix::RejectError::ValueOORange, "Expected: 1 GE value GE 9 || A GE value GE I || value EQ P" };
    }
}

void to_FIX(std::string &_out, const fix42::OrderType _value)
{
    _out.push_back(static_cast<char>(_value));
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, std::string &_out)
{
    _out = _value;
    return std::nullopt;
}

void to_FIX(std::string &_out, const std::string &_value)
{
    _out.append(_value);
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, char &_out)
{
    if (_value.size() > 1)
        return fix::RejectError{ fix::RejectError::ValueOORange, "Expected a char" };
    _out = _value[0];
    return std::nullopt;
}

void to_FIX(std::string &_out, const char _value)
{
    _out.push_back(_value);
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, uint8_t &_out)
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

void to_FIX(std::string &_out, const uint8_t _value)
{
    char buffer[fix::meta::NumericLimit::MaxAlloc<uint8_t>()];
    auto [ptr, _] = std::to_chars(buffer, buffer + sizeof(buffer), _value);

    _out.append(buffer, ptr);
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, uint16_t &_out)
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

void to_FIX(std::string &_out, const uint16_t _value)
{
    char buffer[fix::meta::NumericLimit::MaxAlloc<uint16_t>()];
    auto [ptr, _] = std::to_chars(buffer, buffer + sizeof(buffer), _value);

    _out.append(buffer, ptr);
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, uint32_t &_out)
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

void to_FIX(std::string &_out, const uint32_t _value)
{
    char buffer[fix::meta::NumericLimit::MaxAlloc<uint32_t>()];
    auto [ptr, _] = std::to_chars(buffer, buffer + sizeof(buffer), _value);

    _out.append(buffer, ptr);
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, float &_out)
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

void to_FIX(std::string &_out, const float _value)
{
    char buffer[fix::meta::NumericLimit::MaxAlloc<float, 3>()];
    auto [ptr, _] = std::to_chars(buffer, buffer + sizeof(buffer), _value);

    _out.append(buffer, ptr);
}

std::optional<fix::RejectError> from_FIX(const std::string &_value, std::chrono::time_point<std::chrono::system_clock> &_out)
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

void to_FIX(std::string &_out, const std::chrono::time_point<std::chrono::system_clock> &_value)
{
    std::time_t time = std::chrono::system_clock::to_time_t(_value);
    std::tm tm = *std::gmtime(&time);
    int year = 1900 + tm.tm_year;
    int month = tm.tm_mon + 1;
    int day = tm.tm_mday;
    int hour = tm.tm_hour;
    int minute = tm.tm_min;
    int second = tm.tm_sec;
    char *ptr = nullptr;

    _out.resize(_out.size() + 17);
    ptr = _out.data() + _out.size() - 17;
    ptr[0] = '0' + (year / 1000) % 10;
    ptr[1] = '0' + (year / 100) % 10;
    ptr[2] = '0' + (year / 10) % 10;
    ptr[3] = '0' + (year % 10);
    ptr[4] = '0' + month / 10;
    ptr[5] = '0' + month % 10;
    ptr[6] = '0' + day / 10;
    ptr[7] = '0' + day % 10;
    ptr[8] = '-';
    ptr[9]  = '0' + hour / 10;
    ptr[10] = '0' + hour % 10;
    ptr[11] = ':';
    ptr[12] = '0' + minute / 10;
    ptr[13] = '0' + minute % 10;
    ptr[14] = ':';
    ptr[15] = '0' + second / 10;
    ptr[16] = '0' + second % 10;
}
