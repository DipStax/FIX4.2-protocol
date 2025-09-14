#pragma once

#include <optional>
#include <chrono>

#include "Shared/Message-v2/Enum.hpp"
#include "FIX-Message/RejectError.hpp"

std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::RejectReasonSession &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::TransactionType &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::OrderStatus &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::Side &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::RejectReasonBusiness &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::EncryptionMethod &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::HandleInstance &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, fix42::OrderType &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, std::string &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, char &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, uint8_t &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, uint16_t &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, uint32_t &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, float &_out);
std::optional<fix::RejectError> TagConvertor(const std::string &_value, std::chrono::time_point<std::chrono::system_clock> &_out);
