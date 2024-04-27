#include "Client/Core.hpp"
#include "Client/Processor/OBData.hpp"
#include "Client/Processor/OrderBook.hpp"
#include "Client/Processor/Order.hpp"
#include "Client/Processor/ReportHandler.hpp"
#include "Client/Processor/User.hpp"
#include "Common/Message/Tag.hpp"

Core::Core(const net::Ip &_ip, uint32_t _tcp, uint32_t _udp)
    : m_tcp(_ip, _tcp), m_udp(_ip, _udp)
{
    std::shared_ptr<proc::OrderBook> ob = std::make_shared<proc::OrderBook>();
    std::shared_ptr<proc::OBData> obdata = std::make_shared<proc::OBData>();
    std::shared_ptr<proc::User> user = std::make_shared<proc::User>();
    std::shared_ptr<proc::ReportHandler> report = std::make_shared<proc::ReportHandler>();
    std::shared_ptr<proc::Order> order = std::make_shared<proc::Order>();

    m_proc_tcp.push_back(ob);
    m_proc_tcp.push_back(user);
    m_proc_tcp.push_back(report);

    m_proc_udp.push_back(ob);

    m_proc_entry.push_back(user);
    m_proc_entry.push_back(ob);
    m_proc_entry.push_back(obdata);
    m_proc_entry.push_back(order);
}

Core::~Core()
{
    stop();
}

void Core::start()
{
    m_running = true;

    (void)m_udp.start();
    (void)m_tcp.start();
    (void)m_input.start();

    while (m_running) {
        if (!m_udp.empty(io::Side::Recv)) {
            const data::UDPPackage package = m_udp.pop_front_recv();

            for (auto &_proc : m_proc_udp) {
                std::optional<data::UDPPackage> res = _proc->process(package, m_context);

                if (res.has_value()) {
                    m_udp.send(std::move(res.value()));
                    break;
                }
            }
        }
        if (!m_tcp.empty(io::Side::Recv)) {
            fix::Serializer::AnonMessage val = m_tcp.pop_front_recv();

            for (auto &_proc : m_proc_tcp) {
                if (_proc->handle(val, m_context)) {
                    std::optional<fix::Message> res = _proc->process(val, m_context);

                    if (res.has_value()) {
                        setContext(res.value());
                        m_tcp.send(std::move(res.value()));
                    }
                    break;
                }
            }
        }
        if (!m_input.empty(io::Side::Recv)) {
            const Entry entry = m_input.pop_front_recv();

            for (auto &_proc : m_proc_entry) {
                if (_proc->handle(entry, m_context)) {
                    std::optional<fix::Message> res = _proc->process(entry, m_context);

                    if (res.has_value()) {
                        setContext(res.value());
                        m_tcp.send(std::move(res.value()));
                    }
                    break;
                }
            }
        }
    }
}

void Core::stop()
{
    m_running = false;

    (void)m_udp.stop();
    (void)m_tcp.stop();
    (void)m_input.stop();
}

void Core::setContext(fix::Message &_msg)
{
    _msg.header.set56_TargetCompId(PROVIDER_NAME);
    _msg.header.set34_msgSeqNum(std::to_string(m_context.SeqNum++));
    if (m_context.Loggin)
        _msg.header.set49_SenderCompId(m_context.User);
}