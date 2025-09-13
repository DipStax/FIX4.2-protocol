#pragma once

#include "Server/ProcessUnit/data/Global.hpp"

#include "Shared/ProcessUnit/AInputProcess.hpp"

namespace pu
{
    class HeaderValidation : public AInputProcess<Context<data::UnparsedMessage>>
    {
        public:
            HeaderValidation(UnparsedMessageQueue &_output, StringOutputQueue &_error);
            virtual ~HeaderValidation() = default;

        protected:
            void onInput(InputType _input) final;

            [[nodiscard]] std::optional<fix42::msg::SessionReject> verifyPositionValue(const fix42::Header &_header);
            [[nodiscard]] std::optional<fix42::msg::SessionReject> verifyUserSpecific(const fix42::Header &_header, const ClientStore::Client &_client);

        private:
            UnparsedMessageQueue &m_output;
            StringOutputQueue &m_error;
    };
}