#pragma once

#include "Common/Message/Serializer.hpp"
#include "Common/Thread/Queue.hpp"

using TransitMessage = fix::Serializer::AnonMessage;
using QueueTransit = ts::Queue<TransitMessage>;