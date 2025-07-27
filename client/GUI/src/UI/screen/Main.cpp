#include <QHBoxLayout>
#include <QVBoxLayout>

#include "Client/GUI/UI/screen/Main.hpp"

#include "Client/GUI/UI/widget/OrderExecution.hpp"
#include "Client/GUI/UI/widget/OrderHistoric.hpp"

namespace ui::screen
{
    Main::Main(QWidget *_parent)
        : QWidget(_parent)
    {
        QHBoxLayout *main_layout = new QHBoxLayout();
        QVBoxLayout *left_layout = new QVBoxLayout();

        left_layout->addWidget(new ui::widget::OrderExecution());

        main_layout->addLayout(left_layout);
        main_layout->addWidget(new ui::widget::OrderHistoric());

        setLayout(main_layout);
    }
}