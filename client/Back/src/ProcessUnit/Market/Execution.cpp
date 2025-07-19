#include "Client/Back/ProcessUnit/Market/Execution.hpp"
#include "Client/Back/FrontManager.hpp"

#include "Common/Message/Tag.hpp"

#include "Client/Common/IPC/Message/Execution.hpp"
#include "Client/Common/IPC/Helper.hpp"

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

        switch (_input.at(fix::Tag::ExecTransType)[0]) {
            case '0': treatNewOrder(_input);
                break;
        }
    }

    void Execution::treatNewOrder(const InputType &_input)
    {
        switch (_input.at(fix::Tag::ExecType)[0]) {
            case OrderStatus::New: sendNotificationNew(_input, ipc::MessageType::ExecNew);
                break;
            case OrderStatus::PartiallyFilled:
            case OrderStatus::Filled:
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
            std::stod(_input.at(fix::Tag::Price)),
            utils::to<Side>(_input.at(fix::Tag::Side)),
            utils::to<Quantity>(_input.at(fix::Tag::OrderQty)),
            utils::to<Quantity>(_input.at(fix::Tag::LeavesQty))
        };

        FrontManager::Instance().notify(ipc::Helper::ExecutionEvent(exec, _msgtype));
    }
}