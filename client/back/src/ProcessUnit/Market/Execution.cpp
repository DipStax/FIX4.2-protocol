#include "Client/Back/ProcessUnit/Market/Execution.hpp"
#include "Client/Back/FrontManager.hpp"

#include "Shared/Message/Tag.hpp"

#include "Client/Shared/IPC/Message/Execution.hpp"
#include "Client/Shared/IPC/Helper.hpp"

namespace pu
{
    Execution::Execution(QueueMessage &_tcp_output)
        : AInputProcess<TransitMessage>("Client/Execution"),
        m_tcp_output(_tcp_output)
    {
    }

    void Execution::onInput(InputType _input)
    {
        // std::pair<bool, fix::Reject> reject = fix::ExecutionReport::Verify(_input);

        Logger->log<logger::Level::Verbose>("ExecTransType used: ", _input.at(fix::Tag::ExecTransType)[0]);
        Logger->log<logger::Level::Verbose>("ExecType used: ", _input.at(fix::Tag::ExecType));

        switch (_input.at(fix::Tag::ExecTransType)[0]) {
            case '0': treatNewOrder(_input);
                break;
        }
    }

    void Execution::treatNewOrder(const InputType &_input)
    {
        switch (_input.at(fix::Tag::ExecType)[0]) {
            case ExecTypeValue::New: sendNotificationNew(_input, ipc::MessageType::ExecNew);
                break;
            case ExecTypeValue::PartiallyFilled:
            case ExecTypeValue::Filled:
                sendNotificationNew(_input, ipc::MessageType::ExecEvent);
                break;
        }
    }

    void Execution::sendNotificationNew(const InputType &_input, ipc::MessageType _msgtype)
    {
        ipc::msg::Execution exec{
            _input.at(fix::Tag::Symbol),
            _input.at(fix::Tag::OrderID),
            _input.at(fix::Tag::ExecId),
            std::stod(_input.at(fix::Tag::AvgPx)),
            std::stod(_input.at(fix::Tag::Price)),
            utils::to<Side>(_input.at(fix::Tag::Side)),
            utils::to<Quantity>(_input.at(fix::Tag::OrderQty)),
            utils::to<Quantity>(_input.at(fix::Tag::LeavesQty))
        };

        Logger->log<logger::Level::Info>("Notifying frontend of validated new order {", static_cast<int>(_msgtype), "}: ", exec);
        FrontManager::Instance().send(ipc::Helper::ExecutionEvent(exec, _msgtype));
    }
}