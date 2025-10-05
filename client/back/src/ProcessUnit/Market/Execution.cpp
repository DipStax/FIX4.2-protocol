#include "Client/Back/ProcessUnit/Market/Execution.hpp"
#include "Client/Back/FrontManager.hpp"

#include "Client/Shared/IPC/Message/Execution.hpp"
#include "Client/Shared/IPC/Helper.hpp"
#include "Shared/Core/Enum.hpp"
#include "Shared/Message-v2/Parser.hpp"

namespace pu
{
    Execution::Execution(StringOutputQueue &_tcp_output)
        : AInputProcess<InputType>("Back/Execution"),
        m_tcp_output(_tcp_output)
    {
    }

    void Execution::onInput(InputType _input)
    {
        xstd::Expected<fix42::msg::ExecutionReport, fix42::msg::SessionReject> error = fix42::parseMessage<fix42::msg::ExecutionReport>(_input.Message, _input.Header);

        if (error.has_error()) {
            Logger->log<logger::Level::Info>("Parsing of Logon message failed: ", error.error().get<fix42::tag::Text>().Value.value());
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::SessionReject::Type, std::move(error.error().to_string()));
            return;
        }

        const fix42::msg::ExecutionReport &report = error.value();

        Logger->log<logger::Level::Verbose>("ExecTransType used: ", error.value().get<fix42::tag::ExecTransType>().Value);
        Logger->log<logger::Level::Verbose>("ExecType used: ", error.value().get<fix42::tag::ExecType>().Value);

        // todo Select from ordertype if available (limit)

        switch (report.get<fix42::tag::ExecTransType>().Value) {
            case fix42::TransactionType::New:
                treatNewOrder(_input, report);
                break;
        }
    }

    void Execution::treatNewOrder(const InputType &_input, const fix42::msg::ExecutionReport &_report)
    {
        switch (_report.get<fix42::tag::ExecType>().Value) {
            case fix42::ExecutionStatus::NewOrder:
                notifyExecNew(_input, _report);
                break;
            case fix42::ExecutionStatus::PartiallyFilled:
            case fix42::ExecutionStatus::Filled:
                notifyExecEvent(_input, _report);
                break;
        }
    }

    void Execution::notifyExecNew(const InputType &_input, const fix42::msg::ExecutionReport &_report)
    {
        if (!_report.get<fix42::tag::OrdType>().Value.has_value()) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _report.get<fix42::tag::OrderID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Awaited a value for OrderType in aknowledge report";
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        } else if (!_report.get<fix42::tag::Price>().Value.has_value()) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _report.get<fix42::tag::OrderID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Awaited a value for Price in aknowledge report";
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        } else if (!_report.get<fix42::tag::OrderQty>().Value.has_value()) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _report.get<fix42::tag::OrderID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Awaited a value for OrderQty in aknowledge report";
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        }

        ipc::msg::ExecutionNew exec{
            .orderId = _report.get<fix42::tag::OrderID>().Value,
            .execId =_report.get<fix42::tag::ExecId>().Value,
            .symbol = _report.get<fix42::tag::Symbol>().Value,
            .side = _report.get<fix42::tag::Side>().Value,
            .orderType = _report.get<fix42::tag::OrdType>().Value.value(),
            .quantity = _report.get<fix42::tag::OrderQty>().Value.value(),
            .price = _report.get<fix42::tag::Price>().Value.value()
        };

        Logger->log<logger::Level::Info>("Sending exec new: ", exec);
        FrontManager::Instance().send(ipc::Helper::ExecutionNew(exec));
    }

    void Execution::notifyExecEvent(const InputType &_input, const fix42::msg::ExecutionReport &_report)
    {
        if (!_report.get<fix42::tag::LastShares>().Value.has_value()) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _report.get<fix42::tag::OrderID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Awaited a value for LastShares in generated report";
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        } else if (!_report.get<fix42::tag::LastPx>().Value.has_value()) {
            fix42::msg::BusinessReject reject{};

            reject.get<fix42::tag::RefSeqNum>().Value = _input.Header.get<fix42::tag::MsgSeqNum>().Value;
            reject.get<fix42::tag::RefMsgType>().Value = _input.Header.getPositional<fix42::tag::MsgType>().Value;
            reject.get<fix42::tag::BusinessRejectRefId>().Value = _report.get<fix42::tag::OrderID>().Value;
            reject.get<fix42::tag::BusinessRejectReason>().Value = fix42::RejectReasonBusiness::CondReqFieldMissing;
            reject.get<fix42::tag::Text>().Value = "Awaited a value for LastPrice in generated report";
            m_tcp_output.append(_input.ReceiveTime, fix42::msg::BusinessReject::Type, std::move(reject.to_string()));
            return;
        }

        ipc::msg::ExecutionEvent exec{
            .orderId = _report.get<fix42::tag::OrderID>().Value,
            .execId =_report.get<fix42::tag::ExecId>().Value,
            .execStatus = _report.get<fix42::tag::ExecType>().Value,
            .symbol = _report.get<fix42::tag::Symbol>().Value,
            .side = _report.get<fix42::tag::Side>().Value,
            .lastShare = _report.get<fix42::tag::LastShares>().Value.value(),
            .leaveQty = _report.get<fix42::tag::LeavesQty>().Value,
            .cumQty = _report.get<fix42::tag::CumQty>().Value,
            .avgPrice = _report.get<fix42::tag::AvgPx>().Value,
            .lastPrice = _report.get<fix42::tag::LastPx>().Value.value()
        };

        Logger->log<logger::Level::Info>("Sending exec event: ", exec);
        FrontManager::Instance().send(ipc::Helper::ExecutionEvent(exec));
    }
}