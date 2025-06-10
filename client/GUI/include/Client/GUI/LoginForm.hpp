#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

namespace ui
{
    class LoginForm : public QWidget
    {
        Q_OBJECT

        public:
            LoginForm(QWidget *parent = nullptr)
                : QWidget(parent), m_entry(new QLineEdit())
            {
                QLabel *label = new QLabel("Login:");
                QPushButton *button = new QPushButton("Submit");

                connect(button, &QPushButton::clicked, this, &LoginForm::onSubmit);

                QVBoxLayout *layout = new QVBoxLayout;
                layout->addWidget(label);
                layout->addWidget(m_entry);
                layout->addWidget(button);

                setLayout(layout);
                setWindowTitle("Simple Form");
            }
            ~LoginForm() = default;

        private:
            void onSubmit() {
                    
            }

            QLineEdit *m_entry;
    };
}