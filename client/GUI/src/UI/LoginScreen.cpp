#include <QVBoxLayout>
#include <QLabel>

#include "Client/GUI/UI/LoginScreen.hpp"
#include "Client/GUI/BackManager.hpp"

namespace ui
{
    LoginScreen::LoginScreen(QWidget *parent)
        : QDialog(parent), m_entry(new QLineEdit()), m_button(new QPushButton("Submit"))
    {
        QLabel *label = new QLabel("Login:");

        connect(m_button, &QPushButton::clicked, this, &LoginScreen::onSubmit);
        connect(this, &LoginScreen::requestConnection, BackManager::Instance(), &BackManager::startConnection, Qt::QueuedConnection);

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);
        layout->addWidget(m_entry);
        layout->addWidget(m_button);

        setLayout(layout);
        setWindowTitle("Simple Form");
    }

    void LoginScreen::onSubmit()
    {
        m_entry->setEnabled(false);
        m_button->setEnabled(false);
        emit requestConnection();
    }
}