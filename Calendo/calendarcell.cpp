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
    layout->setSpacing(5);
    layout->setContentsMargins(7, 7, 7, 7);

    dayLabel = new QLabel(QString::number(day), this);
    dayLabel->setStyleSheet("color: #cccccc; font-weight: bold; font-size: 14px;");
    dayLabel->move(10, 5);  // Вручную размещаем в верхнем левом углу

    addEventLabel = new QLabel("Добавить событие?", this);
    addEventLabel->setAlignment(Qt::AlignCenter);
    addEventLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    addEventLabel->setVisible(false);
    addEventLabel->setStyleSheet(R"(
    QLabel {
        color: #aaaaaa;
        font-size: 14px;
        background-color: rgba(36, 38, 41, 150);
        border-radius: 15px;
        padding: 10px;
    }
)");

    layout->addWidget(addEventLabel);  // центрируем и растягиваем

    QDate today = QDate::currentDate();
    QDate cellDate(year, month, day);
    if (today.year() == year && today.month() == month && today.day() == day) {
        isToday = true;
        dayLabel->setStyleSheet("color: #8ecae6; font-weight: bold; font-size: 14px;");
    }
    else if (cellDate.dayOfWeek() == 6 || cellDate.dayOfWeek() == 7){
        dayLabel->setStyleSheet("color: lightcoral; font-weight: bold; font-size: 14px;");
    }
    else {
        isToday = false;
        dayLabel->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
    }

    hoverBackground = "#2f3136"; // Цвет фона при наведении

    updateBackground();
}

void CalendarCell::updateBackground(bool hovered)
{
    QString backgroundColor = hovered ? hoverBackground : normalBackground;
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
        }
    )").arg(backgroundColor));
}

void CalendarCell::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    updateBackground(true);
    addEventLabel->setVisible(true);
    dayLabel->setVisible(false);
}

void CalendarCell::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    updateBackground(false);
    addEventLabel->setVisible(false);
    dayLabel->setVisible(true);
}

void CalendarCell::mousePressEvent(QMouseEvent *event)
{
    event->ignore(); // Делает ячейку некликабельной
}

CalendarCell::~CalendarCell()
{
    delete ui;
}
