#pragma once

#include "Common/Message/Serializer.hpp"
#include "Common/Thread/Queue.hpp"
#include "Common/Message/Message.hpp"

using TransitMessage = fix::Serializer::AnonMessage;
using QueueTransit = ts::Queue<TransitMessage>;
using QueueMessage = ts::Queue<fix::Message>;