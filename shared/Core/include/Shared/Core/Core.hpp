#pragma once

#include <cstdint>
#include <iostream>
#include <list>

using UserId = std::string;
using OrderId = std::string;
using ExecId = std::string;
using Quantity = float;
using Price = float;
using Side = uint8_t;

#include "Shared/Core/Enum.hpp"

struct Order
{
    UserId userId;
    OrderId orderId;
    Quantity originalQty;
    Quantity remainQty;
    Price avgPrice;
    fix42::Side side;
    fix42::OrderStatus status;
};

std::ostream &operator<<(std::ostream &_os, const Order &_order);

using OrderList = std::list<Order>;

namespace core
{
    constexpr const char SideSellStr[] = "Sell";
    constexpr const char SideSellPlusStr[] = "SellPlus";
    constexpr const char SideBuyStr[] = "Buy";
    constexpr const char SideBuyMinusStr[] = "BuyMinus";

    constexpr const char *SideToString(fix42::Side _side)
    {
        switch (_side) {
            case fix42::Side::Buy: return SideBuyStr;
            case fix42::Side::Sell: return SideSellStr;
            case fix42::Side::BuyMinus: return SideBuyMinusStr;
            case fix42::Side::SellPlus: return SideSellPlusStr;
            default: return "unknown";
        }
    }

    constexpr const char OrdStatusNewStr[] = "New";
    constexpr const char OrdStatusPartiallyFilledStr[] = "Partially Filled";
    constexpr const char OrdStatusFilledStr[] = "Filled";
    constexpr const char OrdStatusDoneForDayStr[] = "Done for the Day";
    constexpr const char OrdStatusCanceledStr[] = "Canceled";
    constexpr const char OrdStatusReplacedStr[] = "Replaced";
    constexpr const char OrdStatusPendingCancelStr[] = "Pending Cancel";
    constexpr const char OrdStatusStoppedStr[] = "Stopped";
    constexpr const char OrdStatusRejectedStr[] = "Rejected";
    constexpr const char OrdStatusSuspendedStr[] = "Suspended";
    constexpr const char OrdStatusPendingNewStr[] = "Pending New";
    constexpr const char OrdStatusCalculatedStr[] = "Calculated";
    constexpr const char OrdStatusExpiredStr[] = "Expired";
    constexpr const char OrdStatusAccepteBidingStr[] = "Accepte Biding";
    constexpr const char ExecStatusRestated[] = "Restated";
    constexpr const char OrdStatusPendingReplaceStr[] = "Pending Replace";

    constexpr const char *OrderStatusToString(fix42::OrderStatus _status)
    {
        switch (_status){
            case fix42::OrderStatus::NewOrder: return OrdStatusNewStr;
            case fix42::OrderStatus::PartiallyFilled: return OrdStatusPartiallyFilledStr;
            case fix42::OrderStatus::Filled: return OrdStatusFilledStr;
            case fix42::OrderStatus::DoneForDay: return OrdStatusDoneForDayStr;
            case fix42::OrderStatus::Canceled: return OrdStatusCanceledStr;
            case fix42::OrderStatus::Replaced: return OrdStatusReplacedStr;
            case fix42::OrderStatus::PendingCancel: return OrdStatusPendingCancelStr;
            case fix42::OrderStatus::Stopped: return OrdStatusStoppedStr;
            case fix42::OrderStatus::Rejected: return OrdStatusRejectedStr;
            case fix42::OrderStatus::Suspended: return OrdStatusSuspendedStr;
            case fix42::OrderStatus::PendingNew: return OrdStatusPendingNewStr;
            case fix42::OrderStatus::Caluclated: return OrdStatusCalculatedStr;
            case fix42::OrderStatus::Expired: return OrdStatusExpiredStr;
            case fix42::OrderStatus::AcceptedBidding_Restated: return OrdStatusAccepteBidingStr;
            case fix42::OrderStatus::PendingReplace: return OrdStatusPendingReplaceStr;
            default: return "unknown";
        }
    }

    constexpr const char *ExecStatusToString(fix42::ExecutionStatus _status)
    {
        switch (_status){
            case fix42::ExecutionStatus::NewOrder: return OrdStatusNewStr;
            case fix42::ExecutionStatus::PartiallyFilled: return OrdStatusPartiallyFilledStr;
            case fix42::ExecutionStatus::Filled: return OrdStatusFilledStr;
            case fix42::ExecutionStatus::DoneForDay: return OrdStatusDoneForDayStr;
            case fix42::ExecutionStatus::Canceled: return OrdStatusCanceledStr;
            case fix42::ExecutionStatus::Replaced: return OrdStatusReplacedStr;
            case fix42::ExecutionStatus::PendingCancel: return OrdStatusPendingCancelStr;
            case fix42::ExecutionStatus::Stopped: return OrdStatusStoppedStr;
            case fix42::ExecutionStatus::Rejected: return OrdStatusRejectedStr;
            case fix42::ExecutionStatus::Suspended: return OrdStatusSuspendedStr;
            case fix42::ExecutionStatus::PendingNew: return OrdStatusPendingNewStr;
            case fix42::ExecutionStatus::Caluclated: return OrdStatusCalculatedStr;
            case fix42::ExecutionStatus::Expired: return OrdStatusExpiredStr;
            case fix42::ExecutionStatus::AcceptedBidding_Restated: return ExecStatusRestated;
            case fix42::ExecutionStatus::PendingReplace: return OrdStatusPendingReplaceStr;
            default: return "unknown";
        }
    }
}