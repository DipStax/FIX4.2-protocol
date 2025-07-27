#pragma once

#include <QWidget>

namespace ui::screen
{
    class Main : public QWidget
    {
        Q_OBJECT

        public:
            Main(QWidget *_parent = nullptr);
            ~Main() = default;
    };
}