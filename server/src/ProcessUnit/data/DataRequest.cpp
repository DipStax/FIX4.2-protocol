#include "Server/ProcessUnit/data/DataRequest.hpp"

namespace data
{
    MDSnapshotRequest::MDSnapshotRequest(uint32_t _depth, const std::string &_id)
        : depth(_depth), id(_id)
    {
    }
}