#include <QVBoxLayout>
#include <QTreeView>

#include "Client/GUI/UI/widget/OrderHistoric.hpp"
#include "Client/GUI/BackManager.hpp"

namespace ui::widget
{
    OrderHistoric::OrderHistoric(QWidget *_parent)
        : QWidget(_parent)
    {
        QVBoxLayout *layout = new QVBoxLayout();
        QTreeView *treeview = new QTreeView();
        m_model.setHorizontalHeaderLabels({ "Order Id", "Exec Id", "Original Qty", "Remain Qty", "Limit Price", "Avg Price", "Side", "Symbol" });

        // Top-level item (main)
        // QList<QStandardItem*> parentRow;
        // parentRow << new QStandardItem("Main A") << new QStandardItem("Value A1") << new QStandardItem("Value A2");

        // QStandardItem *parentItem = parentRow[0];
        // model.appendRow(parentRow);

        // // Child item (sub)
        // QList<QStandardItem*> childRow;
        // childRow << new QStandardItem("Sub A") << new QStandardItem("Value B1") << new QStandardItem("Value B2");

        // parentItem->appendRow(childRow);

        treeview->setModel(&m_model);
        treeview->expandAll();

        connect(BackManager::Instance(), &BackManager::received_ExecutionNew, this, &OrderHistoric::newOrder);
        // connect(BackManager::Instance(), &BackManager::received_ExecutionEvent, this, &OrderHistoric::orderEvent);

        layout->addWidget(treeview);
        setLayout(layout);
    }

    void OrderHistoric::newOrder(ipc::msg::Execution _order)
    {
        m_root[_order.orderId]
            << new QStandardItem(QString::fromStdString(_order.orderId))
            << new QStandardItem(QString::fromStdString(_order.execId))
            << new QStandardItem(QString::fromStdString(std::to_string(_order.quantity)))
            << new QStandardItem(QString::fromStdString(std::to_string(_order.remainQty)))
            << new QStandardItem(QString::fromStdString(std::to_string(_order.price)))
            << new QStandardItem("0")
            << new QStandardItem(_order.side == static_cast<uint8_t>(OrderType::Ask) ? "Ask" : "Bid")
            << new QStandardItem(QString::fromStdString(_order.symbol));
        m_model.appendRow(m_root.at(_order.orderId));
    }
}