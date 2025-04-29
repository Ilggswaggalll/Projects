#include "calendarcell.h"
#include "ui_calendarcell.h"
#include <QEvent>
#include <QPalette>
#include <QDate>
#include <QMouseEvent>

CalendarCell::CalendarCell(int year, int month, int day, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CalendarCell)
{
    ui->setupUi(this);

    layout = new QVBoxLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(10, 10, 10, 10);

    dayLabel = new QLabel(QString::number(day), this);
    dayLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    dayLabel->setStyleSheet("color: #cccccc; font-weight: bold; font-size: 14px;");

    addEventLabel = new QLabel("Добавить событие?", this);
    addEventLabel->setAlignment(Qt::AlignCenter);
    addEventLabel->setStyleSheet("color: #aaaaaa; font-size: 12px;");
    addEventLabel->setVisible(false);

    layout->addWidget(dayLabel);
    layout->addStretch();
    layout->addWidget(addEventLabel);

    QDate today = QDate::currentDate();
    if (today.year() == year && today.month() == month && today.day() == day) {
        isToday = true;
        normalBackground = "#5c5f7a";  // Светлый фон для сегодняшнего дня
    } else {
        isToday = false;
        normalBackground = "#2f3136";  // Обычный фон
    }

    hoverBackground = "#6a6f90"; // Цвет фона при наведении

    updateBackground();
}

void CalendarCell::updateBackground(bool hovered)
{
    QString backgroundColor = hovered ? hoverBackground : normalBackground;
    this->setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border-radius: 10px;
        }
    )").arg(backgroundColor));
}

void CalendarCell::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    updateBackground(true);
    addEventLabel->setVisible(true);
}

void CalendarCell::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    updateBackground(false);
    addEventLabel->setVisible(false);
}

void CalendarCell::mousePressEvent(QMouseEvent *event)
{
    event->ignore(); // Делает ячейку некликабельной
}

CalendarCell::~CalendarCell()
{
    delete ui;
}
