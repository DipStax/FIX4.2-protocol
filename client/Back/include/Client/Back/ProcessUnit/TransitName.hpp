#pragma once

#include "Shared/Message/Serializer.hpp"
#include "Shared/Thread/Queue.hpp"
#include "Shared/Message/Message.hpp"

using TransitMessage = fix::Serializer::AnonMessage;
using QueueTransit = ts::Queue<TransitMessage>;
using QueueMessage = ts::Queue<fix::Message>;