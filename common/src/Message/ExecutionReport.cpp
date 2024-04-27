#include "Common/Message/ExecutionReport.hpp"
#include "Common/Message/Tag.hpp"

namespace fix
{
    ExecutionReport::ExecutionReport()
    {
        header.set35_MsgType(MsgType);
    }

    void ExecutionReport::set6_avgPx(const std::string &_val)
    {
        m_params.append({ Tag::AvgPx, _val });
    }

    void ExecutionReport::set14_cumQty(const std::string &_val)
    {
        m_params.append({ Tag::CumQty, _val });
    }

    void ExecutionReport::set17_execID(const std::string &_val)
    {
        m_params.append({ Tag::ExecId, _val });
    }

    void ExecutionReport::set17_execID()
    {
        m_params.append({ Tag::ExecId, std::to_string(ExecId++) });
    }

    void ExecutionReport::set20_execTransType(const std::string &_val)
    {
        m_params.append({ Tag::ExecTransType, _val });
    }

    void ExecutionReport::set37_orderID(const std::string &_val)
    {
        m_params.append({ Tag::OrderID, _val });
    }

    void ExecutionReport::set38_orderQty(const std::string &_val)
    {
        m_params.append({ Tag::OrderQty, _val });
    }

    void ExecutionReport::set39_ordStatus(const std::string &_val)
    {
        m_params.append({ Tag::OrdStatus, _val });
    }

    void ExecutionReport::set40_ordType(const std::string &_val)
    {
        m_params.append({ Tag::OrdType, _val });
    }

    void ExecutionReport::set41_origClOrdID(const std::string &_val)
    {
        m_params.append({ Tag::OrigClOrdID, _val });
    }

    void ExecutionReport::set44_price(const std::string &_val)
    {
        m_params.append({ Tag::Price, _val });
    }

    void ExecutionReport::set54_side(const std::string &_val)
    {
        m_params.append({ Tag::Side, _val });
    }

    void ExecutionReport::set55_symbol(const std::string &_val)
    {
        m_params.append({ Tag::Symbol, _val });
    }

    void ExecutionReport::set58_text(const std::string &_val)
    {
        m_params.append({ Tag::Text, _val });
    }

    void ExecutionReport::set150_execType(const std::string &_val)
    {
        m_params.append({ Tag::ExecType, _val });
    }

    void ExecutionReport::set151_leavesQty(const std::string &_val)
    {
        m_params.append({ Tag::LeavesQty, _val });
    }
}