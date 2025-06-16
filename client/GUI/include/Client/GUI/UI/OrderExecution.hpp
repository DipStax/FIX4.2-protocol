#pragma once

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>

namespace ui
{
    class OrderExecution : public QWidget
    {
        Q_OBJECT

        public:
            OrderExecution(QWidget *_parent = nullptr);
            ~OrderExecution() = default;

        private:
            QVBoxLayout *m_main_lyaout = nullptr;

            QComboBox *m_cb_side = nullptr;
            QLineEdit *m_entry_orderid = nullptr;
            QLineEdit *m_entry_qty = nullptr;
            QLineEdit *m_entry_price = nullptr;

            QPushButton *m_button = nullptr;
    };
}