#include <QVBoxLayout>
#include <QTreeView>

#include "Client/GUI/UI/widget/OrderHistoric.hpp"
#include "Client/GUI/BackManager.hpp"

#include "Shared/Log/Manager.hpp"

namespace ui::widget
{
    OrderHistoric::OrderHistoric(QWidget *_parent)
        : QWidget(_parent), Logger(logger::Manager::newLogger("OrderHistoric"))
    {
        QVBoxLayout *layout = new QVBoxLayout();
        QTreeView *treeview = new QTreeView();
        m_model.setHorizontalHeaderLabels({ "Order Id", "Limit Price", "Avg Price", "Quantity", "Status", "Symbol", "Remain Qty", "Side", "Exec Id" });

        treeview->setModel(&m_model);
        treeview->expandAll();

        connect(BackManager::Instance(), &BackManager::received_ExecutionNew, this, &OrderHistoric::newOrder);
        connect(BackManager::Instance(), &BackManager::received_ExecutionEvent, this, &OrderHistoric::eventOrder);

        layout->addWidget(treeview);
        setLayout(layout);
    }

    void OrderHistoric::newOrder(ipc::msg::ExecutionNew _exec)
    {
        Logger->log<logger::Level::Info>("New order: ", _exec);
        QList<QStandardItem*> row;
        QStandardItem *quantity = new QStandardItem(QString::fromStdString(std::to_string(_exec.quantity)));
        QStandardItem *remainQty = new QStandardItem(QString::fromStdString(std::to_string(_exec.quantity)));
        QStandardItem *avgPrice = new QStandardItem(QString::fromStdString(std::to_string(0)));

        quantity->setData(_exec.quantity);
        remainQty->setData(_exec.quantity);
        avgPrice->setData(0);
        row << new QStandardItem(QString::fromStdString(_exec.orderId))
            << new QStandardItem(QString::fromStdString(std::to_string(_exec.price)))
            << avgPrice
            << quantity
            << new QStandardItem(core::ExecStatusToString(fix42::ExecutionStatus::NewOrder))
            << new QStandardItem(QString::fromStdString(_exec.symbol))
            << remainQty
            << new QStandardItem(core::SideToString(_exec.side))
            << new QStandardItem(QString::fromStdString(_exec.execId));
        for (QStandardItem* item : row)
            item->setEditable(false);
        m_model.appendRow(row);
        m_root[_exec.orderId] = row;
    }

    void OrderHistoric::eventOrder(ipc::msg::ExecutionEvent _exec)
    {
        Logger->log<logger::Level::Info>("New order event: ", _exec);
        QList<QStandardItem*> row;

        if (!m_root.contains(_exec.orderId)) {
            Logger->log<logger::Level::Warning>("Unable to find the root element for order id: ", _exec.orderId);
            return;
        }
        QList<QStandardItem*> &main_row = m_root.at(_exec.orderId);

        row << new QStandardItem("")
            << new QStandardItem("")
            << new QStandardItem(QString::fromStdString(std::to_string(_exec.avgPrice)))
            << new QStandardItem("")
            << new QStandardItem(core::ExecStatusToString(_exec.execStatus))
            << new QStandardItem("")
            << new QStandardItem(QString::fromStdString(std::to_string(_exec.leaveQty)))
            << new QStandardItem(QString::fromStdString(_exec.execId));
        for (QStandardItem* item : row)
            item->setEditable(false);
        main_row[0]->appendRow(row);
    }
}