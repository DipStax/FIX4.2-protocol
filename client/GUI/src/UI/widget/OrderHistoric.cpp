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
        connect(BackManager::Instance(), &BackManager::received_ExecutionEvent, this, &OrderHistoric::eventOrder);

        layout->addWidget(treeview);
        setLayout(layout);
    }

    void OrderHistoric::newOrder(ipc::msg::Execution _order)
    {
        Logger->log<logger::Level::Info>("New order: ", _order);
        QList<QStandardItem*> row;
        QStandardItem *quantity = new QStandardItem(QString::fromStdString(std::to_string(_order.quantity)));
        QStandardItem *remainQty = new QStandardItem(QString::fromStdString(std::to_string(_order.remainQty)));
        QStandardItem *avgPrice = new QStandardItem(QString::fromStdString(std::to_string(_order.avgPrice)));

        quantity->setData(_order.quantity);
        remainQty->setData(_order.remainQty);
        avgPrice->setData(_order.avgPrice);
        row << new QStandardItem(QString::fromStdString(_order.orderId))
            << new QStandardItem(QString::fromStdString(_order.execId))
            << quantity
            << remainQty
            << new QStandardItem(QString::fromStdString(std::to_string(_order.price)))
            << avgPrice
            << new QStandardItem(_order.side == static_cast<uint8_t>(OrderType::Ask) ? "Ask" : "Bid")
            << new QStandardItem(QString::fromStdString(_order.symbol));
        for (QStandardItem* item : row)
            item->setEditable(false);
        m_model.appendRow(row);
        m_root[_order.orderId] = row;
    }

    void OrderHistoric::eventOrder(ipc::msg::Execution _order)
    {
        Logger->log<logger::Level::Info>("New order event: ", _order);
        QList<QStandardItem*> row;

        if (!m_root.contains(_order.orderId)) {
            Logger->log<logger::Level::Warning>("Unable to find the root element for order id: ", _order.orderId);
            return;
        }
        QList<QStandardItem*> &main_row = m_root.at(_order.orderId);

        row << new QStandardItem("")
            << new QStandardItem(QString::fromStdString(_order.execId))
            << new QStandardItem(QString::fromStdString(std::to_string(_order.quantity)))
            << new QStandardItem(QString::fromStdString(std::to_string(_order.remainQty)))
            << new QStandardItem(QString::fromStdString(std::to_string(_order.price)))
            << new QStandardItem(QString::fromStdString(std::to_string(_order.avgPrice)))
            << new QStandardItem(_order.side == static_cast<uint8_t>(OrderType::Ask) ? "Ask" : "Bid")
            << new QStandardItem(QString::fromStdString(_order.symbol));
        for (QStandardItem* item : row)
            item->setEditable(false);
        main_row[0]->appendRow(row);

        QStandardItem *tl_quantity_row = main_row[2];
        QStandardItem *tl_remain_row = main_row[3];
        QStandardItem *tl_avgprice_row = main_row[5];
        Quantity qty1 = tl_quantity_row->data().toInt() - tl_remain_row->data().toInt();
        Quantity qty2 = _order.quantity - _order.remainQty;
        Quantity result = (qty1 * tl_avgprice_row->data().toInt() + qty2 * _order.avgPrice) / tl_quantity_row->data().toInt();

        tl_avgprice_row->setText(QString::fromStdString(std::to_string(result)));
        tl_avgprice_row->setData(result);
        tl_remain_row->setText(QString::fromStdString(std::to_string(_order.remainQty)));
        tl_remain_row->setData(_order.remainQty);
    }
}