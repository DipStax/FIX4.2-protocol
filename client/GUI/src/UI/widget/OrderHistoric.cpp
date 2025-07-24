#include <QVBoxLayout>
#include <QTreeView>

#include "Client/GUI/UI/widget/OrderHistoric.hpp"
#include "Client/GUI/BackManager.hpp"

#include "Common/Log/Manager.hpp"

namespace ui::widget
{
    OrderHistoric::OrderHistoric(QWidget *_parent)
        : QWidget(_parent), Logger(logger::Manager::newLogger("OrderHistoric"))
    {
        QVBoxLayout *layout = new QVBoxLayout();
        QTreeView *treeview = new QTreeView();
        m_model.setHorizontalHeaderLabels({ "Order Id", "Exec Id", "Original Qty", "Remain Qty", "Limit Price", "Avg Price", "Side", "Symbol" });

        treeview->setModel(&m_model);
        treeview->expandAll();

        connect(BackManager::Instance(), &BackManager::received_ExecutionNew, this, &OrderHistoric::newOrder);

        layout->addWidget(treeview);
        setLayout(layout);
    }

    void OrderHistoric::newOrder(ipc::msg::Execution _order)
    {
        Logger->log<logger::Level::Info>("New order: ", _order);
        QList<QStandardItem*> row;

        row << new QStandardItem(QString::fromStdString(_order.orderId))
            << new QStandardItem(QString::fromStdString(_order.execId))
            << new QStandardItem(QString::fromStdString(std::to_string(_order.quantity)))
            << new QStandardItem(QString::fromStdString(std::to_string(_order.remainQty)))
            << new QStandardItem(QString::fromStdString(std::to_string(_order.price)))
            << new QStandardItem("0")
            << new QStandardItem(_order.side == static_cast<uint8_t>(OrderType::Ask) ? "Ask" : "Bid")
            << new QStandardItem(QString::fromStdString(_order.symbol));
        m_model.appendRow(row);
        m_root[_order.orderId] = row;
    }
}