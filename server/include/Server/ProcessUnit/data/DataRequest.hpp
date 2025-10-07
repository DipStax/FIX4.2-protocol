#pragma once

#include <vector>

#include "Shared/Core/Core.hpp"

namespace data
{
    struct MDSnapshotRequest
    {
        MDSnapshotRequest() = default;
        MDSnapshotRequest(uint32_t _depth, const std::string &_id);
        MDSnapshotRequest(const MDSnapshotRequest &_mdreq) = default;
        virtual ~MDSnapshotRequest() = default;

        std::vector<fix42::MarketDataEntryType> entries{};
        uint32_t depth = 0;
        std::string id{};
    };
}