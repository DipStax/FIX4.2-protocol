#pragma once

#include "Client/Processor/IProcessor.hpp"

namespace proc
{
    class ReportHandler: public proc::IMessage
    {
        public:
            ReportHandler() = default;
            ~ReportHandler() = default;

            [[nodiscard]] virtual bool handle(fix::Serializer::AnonMessage &_msg, const Context &_ctx) const override final;

            [[nodiscard]] std::optional<fix::Message> process(fix::Serializer::AnonMessage &_msg, Context &_ctx) override final;

            [[nodiscard]] virtual std::optional<fix::Message> build(char _tag, const Context &_ctx) const override final;

        protected:
            void treatExecutionReport(fix::Serializer::AnonMessage &_msg, Context &_ctx);
    };
}