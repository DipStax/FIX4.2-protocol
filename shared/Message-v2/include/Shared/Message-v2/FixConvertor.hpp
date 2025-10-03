#pragma once

#include <optional>
#include <chrono>

#include "Shared/Core/Enum.hpp"
#include "FIX-Message/RejectError.hpp"

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::RejectReasonSession &_out);
void to_FIX(std::string &_out, const fix42::RejectReasonSession _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::TransactionType &_out);
void to_FIX(std::string &_out, const fix42::TransactionType _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::OrderStatus &_out);
void to_FIX(std::string &_out, const fix42::OrderStatus _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::OrderRejectReason &_out);
void to_FIX(std::string &_out, const fix42::OrderRejectReason _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::Side &_out);
void to_FIX(std::string &_out, const fix42::Side _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::RejectReasonBusiness &_out);
void to_FIX(std::string &_out, const fix42::RejectReasonBusiness _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::EncryptionMethod &_out);
void to_FIX(std::string &_out, const fix42::EncryptionMethod _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::HandleInstance &_out);
void to_FIX(std::string &_out, const fix42::HandleInstance _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::OrderType &_out);
void to_FIX(std::string &_out, const fix42::OrderType _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::CancelRejectResponseTo &_out);
void to_FIX(std::string &_out, const fix42::CancelRejectResponseTo &_value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, fix42::CancelRejectReason &_out);
void to_FIX(std::string &_out, const fix42::CancelRejectReason &_value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, std::string &_out);
void to_FIX(std::string &_out, const std::string &_value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, char &_out);
void to_FIX(std::string &_out, const char _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, uint8_t &_out);
void to_FIX(std::string &_out, const uint8_t _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, uint16_t &_out);
void to_FIX(std::string &_out, const uint16_t _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, uint32_t &_out);
void to_FIX(std::string &_out, const uint32_t _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, float &_out);
void to_FIX(std::string &_out, const float _value);

std::optional<fix::RejectError> from_FIX(const std::string &_value, std::chrono::time_point<std::chrono::system_clock> &_out);
void to_FIX(std::string &_out, const std::chrono::time_point<std::chrono::system_clock> &_value);

