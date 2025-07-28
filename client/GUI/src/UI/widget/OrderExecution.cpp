#include <QHBoxLayout>
#include <QLabel>

#include "Client/GUI/UI/widget/OrderExecution.hpp"
#include "Client/GUI/BackManager.hpp"

#include "Client/Shared/IPC/Helper.hpp"
#include "Client/Shared/IPC/Message/OrderSingle.hpp"

#include "Shared/Log/Manager.hpp"
#include "Shared/Core/Order.hpp"

namespace ui::widget
{
    OrderExecution::OrderExecution(QWidget *_parent)
        : QWidget(_parent),
        m_main_lyaout(new QVBoxLayout()),
        m_cb_symbol(new QComboBox()),
        m_cb_side(new QComboBox()),
        m_entry_orderid(new QLineEdit()),
        m_entry_qty(new QLineEdit()),
        m_entry_price(new QLineEdit()),
        m_button(new QPushButton("Send")),
        Logger(logger::Manager::newLogger("OrderExecution"))
    {
        QVBoxLayout *left_layout = new QVBoxLayout();
        QVBoxLayout *right_layout = new QVBoxLayout();
        QHBoxLayout *top_layout = new QHBoxLayout();
        std::vector<std::string> symbols{ MARKET_NAME };

        for (const std::string &_symbol : symbols)
            m_cb_symbol->addItem(QString::fromStdString(_symbol));

        m_cb_side->addItem("Ask", static_cast<int>(OrderType::Ask));
        m_cb_side->addItem("Bid", static_cast<int>(OrderType::Bid));

        left_layout->addWidget(new QLabel("Symbol:"));
        left_layout->addWidget(m_cb_symbol);
        left_layout->addWidget(new QLabel("Side:"));
        left_layout->addWidget(m_cb_side);

        m_entry_price->setValidator(new QDoubleValidator(0, 2000000000, 2));
        m_entry_qty->setValidator(new QDoubleValidator(0, 2000000000, 2));

        right_layout->addWidget(new QLabel("Order Id:"));
        right_layout->addWidget(m_entry_orderid);
        right_layout->addWidget(new QLabel("Quantity:"));
        right_layout->addWidget(m_entry_qty);
        right_layout->addWidget(new QLabel("Price:"));
        right_layout->addWidget(m_entry_price);

        top_layout->addLayout(left_layout);
        top_layout->addLayout(right_layout);

        m_main_lyaout->addLayout(top_layout);
        m_main_lyaout->addWidget(m_button);

        connect(m_button, &QPushButton::clicked, this, &OrderExecution::onSubmit);

        setLayout(m_main_lyaout);
    }

    void OrderExecution::onSubmit()
    {
        ipc::msg::OrderSingle order{
            m_cb_symbol->currentText().toStdString(),
            m_entry_orderid->text().toStdString(),
            std::stof(m_entry_price->text().toStdString()),
            static_cast<Quantity>(std::stoul(m_entry_qty->text().toStdString())),
            static_cast<OrderType>(m_cb_side->currentData().toInt())
        };

        Logger->log<logger::Level::Info>("Notifiying server of new order: ", order);
        BackManager::Instance()->send(ipc::Helper::OrderSingle(order));
    }
}

