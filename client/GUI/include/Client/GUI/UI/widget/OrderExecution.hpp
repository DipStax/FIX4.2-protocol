#pragma once

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>

#include "Common/Log/ILogger.hpp"

namespace ui::widget
{
    class OrderExecution : public QWidget
    {
        Q_OBJECT

        public:
            OrderExecution(QWidget *_parent = nullptr);
            ~OrderExecution() = default;

        private:
            void onSubmit();

            QVBoxLayout *m_main_lyaout = nullptr;

            QComboBox *m_cb_symbol = nullptr;
            QComboBox *m_cb_side = nullptr;
            QLineEdit *m_entry_orderid = nullptr;
            QLineEdit *m_entry_qty = nullptr;
            QLineEdit *m_entry_price = nullptr;

            QPushButton *m_button = nullptr;

            std::unique_ptr<logger::ILogger> Logger = nullptr;
    };
}