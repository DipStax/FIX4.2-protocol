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
                treatNewOrder(report);
                break;
        }
    }

    void Execution::treatNewOrder(const fix42::msg::ExecutionReport &_report)
    {
        switch (_report.get<fix42::tag::ExecType>().Value) {
            case fix42::ExecutionStatus::NewOrder:
                sendNotificationNew(_report, ipc::MessageType::ExecNew);
                break;
            case fix42::ExecutionStatus::PartiallyFilled:
            case fix42::ExecutionStatus::Filled:
                sendNotificationNew(_report, ipc::MessageType::ExecEvent);
                break;
        }
    }

    void Execution::sendNotificationNew(const fix42::msg::ExecutionReport &_report, ipc::MessageType _msgtype)
    {
        ipc::msg::Execution exec{
            _report.get<fix42::tag::Symbol>().Value,
            _report.get<fix42::tag::OrderID>().Value,
            _report.get<fix42::tag::ExecId>().Value,
            _report.get<fix42::tag::AvgPx>().Value,
            _report.get<fix42::tag::Price>().Value.value(),
            _report.get<fix42::tag::Side>().Value,
            _report.get<fix42::tag::OrderQty>().Value.value(),
            _report.get<fix42::tag::LeavesQty>().Value
        };

        Logger->log<logger::Level::Info>("Notifying frontend of validated new order {", static_cast<int>(_msgtype), "}: ", exec);
        FrontManager::Instance().send(ipc::Helper::ExecutionEvent(exec, _msgtype));
    }
}