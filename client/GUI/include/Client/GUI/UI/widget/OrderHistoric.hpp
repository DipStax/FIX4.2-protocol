#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>

#include "Client/Common/IPC/Message/Execution.hpp"

#include "Common/Core/Order.hpp"
#include "Common/Log/ILogger.hpp"

namespace ui::widget
{
    class OrderHistoric : public QWidget
    {
        Q_OBJECT

        public:
            OrderHistoric(QWidget *_parent = nullptr);
            ~OrderHistoric() = default;

        public slots:
            void newOrder(ipc::msg::Execution _order);
            void eventOrder(ipc::msg::Execution _order);

        private:
            QStandardItemModel m_model;

            std::map<OrderId, QList<QStandardItem *>> m_root;

            std::unique_ptr<logger::ILogger> Logger;
    };
}