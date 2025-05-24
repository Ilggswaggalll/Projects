#include "calendarcell.h"
#include "ui_calendarcell.h"
#include <QEvent>
#include <QPalette>
#include <QDate>
#include <QMouseEvent>



#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QDate>
#include <QMouseEvent>
#include <QRegularExpression>
#include <QGridLayout>

QDate date;


CalendarCell::CalendarCell(int year, int month, int day, bool isCurrentMonthDay, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CalendarCell)
    , isCurrentMonthDay(isCurrentMonthDay)
    , year(year), month(month), day(day)
    , bellState(-1)
{
    ui->setupUi(this);



    hintLabel = new QLabel(this);
    hintLabel->setText(R"(
    <span style="
        display: inline-block;
        background-color: #ffffff;
        color: #2d2d2d;
        border: 1px solid #555;
        border-radius: 10px;
        padding: 2px 6px;
        font-size: 12px;
        font-weight: bold;
    ">Enter</span><b> для сохранения</b>
)");

    hintLabel->setStyleSheet(R"(
    QLabel {
        color: #c2c2c2;
        font-size: 13px;
    }
)");

    hintLabel->setVisible(false);




    layout = new QVBoxLayout(this);
    //layout = new QGridLayout(this);
    layout->setSpacing(5);
    layout->setContentsMargins(7, 7, 7, 7);

    layout->setAlignment(Qt::AlignTop);

    dayLabel = new QLabel(QString::number(day), this);
    dayLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);


    // Создаем горизонтальный layout для верхней строки (дата + кнопка)
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);

    // Создаем кнопку для правого угла
    cornerButton = new QPushButton(this);
    cornerButton->setFixedSize(20, 20); // Установите нужный размер
    cornerButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            color: white;
            font-weight: bold;
        }
        QPushButton:hover {
            color: #8ecae6;
        }
    )");
    cornerButton->setIcon(QIcon());
    cornerButton->setIconSize(QSize(18, 18));
    cornerButton->setFixedSize(18, 18);

    connect(cornerButton, &QPushButton::clicked, this, &CalendarCell::toggleBellIcon);

    // Добавляем виджеты в горизонтальный layout
    topLayout->addWidget(dayLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    topLayout->addStretch(); // Растягивающийся элемент между датой и кнопкой
    topLayout->addWidget(cornerButton, 0, Qt::AlignRight | Qt::AlignTop);




    addEventButton = new QPushButton("Добавить событие?", this);
    addEventButton->setCursor(Qt::PointingHandCursor);
    addEventButton->setVisible(false);

    addEventButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    addEventButton->setStyleSheet(R"(
        QPushButton {
            color: #aaaaaa;
            font-size: 14px;
            background-color: rgba(36, 38, 41, 150);
            border-radius: 15px;
            padding: 10px;
            border: none;
        }
        QPushButton:hover {
            background-color: rgba(36, 38, 41, 180);
        }
    )");

    connect(addEventButton, &QPushButton::clicked, this, &CalendarCell::showEventEditor);


    eventDisplayButton = new QPushButton(this);
    eventDisplayButton->setVisible(false);
    eventDisplayButton->setCursor(Qt::PointingHandCursor);
    //eventDisplayButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    eventDisplayButton->setStyleSheet(R"(
QPushButton {
color: white;
font-size: 14px;
background-color: transparent;
border: 1px solid #c2e0ed;
border-radius: 8px;
padding: 5px;
text-align: left;
}
QPushButton:hover {
background-color: rgba(36, 38, 41, 150);
border: 1px solid #8ecae6;
}
)");

    connect(eventDisplayButton, &QPushButton::clicked, this, &CalendarCell::showEventEditor);
    eventEditor = new CustomTextEdit(this);
    eventEditor->setVisible(false);
    eventEditor->setStyleSheet(R"(
        QTextEdit {
            color: white;
            font-size: 14px;
            background-color: transparent;
            border: 1px solid #8ecae6;
            border-radius: 8px;
            padding: 5px;
        }
    )");

    connect(eventEditor, &CustomTextEdit::enterPressed, this, [=]() {
        this->hideEventEditor();  // закрываем редактор по Enter
        eventEditor->saveEventsToJson("events1.json");
        QStringList events = eventEditor->getEventList();

        if (!events.isEmpty()) {
            QStringList withBullets;
            for (const QString &text : events) {
                withBullets << "• " + text;
            }
            eventDisplayButton->setText(withBullets.join("\n"));
            eventDisplayButton->setVisible(true);
            addEventButton->setVisible(false);
            dayLabel->setVisible(true);
            //cornerButton->setIcon(QIcon(":/new/prefix1/bell_0.png"));

        } else {
            eventEditor->clear();
            eventDisplayButton->setVisible(false);
            //cornerButton->setIcon(QIcon(":/new/prefix1/bell_0.png"));
            if (this->underMouse()) {
                addEventButton->setVisible(true);
                dayLabel->setVisible(true);
            } else {
                dayLabel->setVisible(true);
            }
        }
    });
    layout->addLayout(topLayout);
    layout->addWidget(eventDisplayButton, 0, Qt::AlignTop); // ← сразу под датой
    layout->addWidget(addEventButton);
    layout->addWidget(eventEditor);
    layout->addWidget(hintLabel);


    //layout->addWidget(dayLabel, 0, 0, 1, 2, Qt::AlignLeft | Qt::AlignTop);
    //layout->addWidget(eventDisplayButton, 1, 0, 1, 2);
    //layout->addWidget(addEventButton, 1, 0, 1, 2);
    //layout->addWidget(eventEditor, 2, 0, 1, 2);


    QDate today = QDate::currentDate();
    QDate cellDate(year, month, day);

    if (!isCurrentMonthDay) {
        dayLabel->setStyleSheet("color: gray; font-weight: bold; font-size: 14px;");
        addEventButton->setVisible(false);
        setEnabled(false);
    } else if (today == QDate(year, month, day)) {
        isToday = true;
        dayLabel->setStyleSheet("color: #8ecae6; font-weight: bold; font-size: 14px;");
    } else if (cellDate.dayOfWeek() == 6 || cellDate.dayOfWeek() == 7) {
        dayLabel->setStyleSheet("color: lightcoral; font-weight: bold; font-size: 14px;");
    } else {
        isToday = false;
        dayLabel->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
    }

    hoverBackground = "#2f3136";






    if (isCurrentMonthDay) {
        QDate cellDate(year, month, day);
        eventEditor->setDate(cellDate);
        eventEditor->loadEventsFromJson("events1.json");
        QStringList events = eventEditor->getEventList();
        int loadedBellState = eventEditor->getBellState();
        bellState = loadedBellState;

        if (!events.isEmpty()) {
            QStringList withBullets;
            for (const QString &text : events) {
                withBullets << "• " + text;
            }
            eventDisplayButton->setText(withBullets.join("\n"));
            eventDisplayButton->setVisible(true);
            addEventButton->setVisible(false);
            if (loadedBellState == 0) {
                cornerButton->setIcon(QIcon(":/new/prefix1/bell_0.png"));
                cornerButton->setToolTip("Напомнить о событии?");
            } else if (loadedBellState == 2) {
                cornerButton->setIcon(QIcon(":/new/prefix1/bell_2.png"));
                cornerButton->setToolTip("Удалить напоминание?");
            }
        }
    }
    updateBackground();
}


void CalendarCell::updateBackground(bool hovered)
{
    if (!isCurrentMonthDay) return;
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
    if (!isCurrentMonthDay || eventEditor->isVisible()) return;
    Q_UNUSED(event);
    updateBackground(true);
    if (!eventDisplayButton->isVisible()){
        addEventButton->setVisible(true);
        //dayLabel->setVisible(false);
    }
}

void CalendarCell::leaveEvent(QEvent *event)
{
    if (!isCurrentMonthDay) return;
    Q_UNUSED(event);
    updateBackground(false);
    addEventButton->setVisible(false);
    //dayLabel->setVisible(true);
}


void CalendarCell::showEventEditor()
{
    if (!eventEditor->isVisible()) {
        emit closeEventEditor();  // закроет редакторы в других ячейках
    }
    QDate cellDate(year, month, day);
    eventEditor->setDate(cellDate);
    eventEditor->loadEventsFromJson("events1.json");
    //QString savedText = eventEditor->toPlainText().trimmed();
    eventDisplayButton->setVisible(false);
    addEventButton->setVisible(false);
    emit closeEventEditor();  // Отправляем сигнал закрытия редакторов в других ячейках
    eventEditor->setVisible(true);
    eventEditor->setFocus();
    hintLabel->setVisible(true);

}


// Проверка, виден ли редактор
bool CalendarCell::isEventEditorVisible() const {
    return eventEditor->isVisible();
}

// Скрыть редактор
void CalendarCell::hideEventEditor() {
    eventEditor->setVisible(false);
    hintLabel->setVisible(false);
    QStringList events = eventEditor->getEventList();
    bellState = eventEditor->getBellState();

    if (!events.isEmpty()) {
        QStringList withBullets;
        for (const QString &text : events) {
            withBullets << "• " + text;
        }
        eventEditor->saveEventsToJson("events1.json");
        eventDisplayButton->setText(withBullets.join("\n"));
        eventDisplayButton->setVisible(true);

        // Устанавливаем иконку только если она еще не установлена
        if (cornerButton->icon().isNull()) {
            cornerButton->setIcon(QIcon(":/new/prefix1/bell_0.png"));
            cornerButton->setToolTip("Напомнить о событии?");
            bellState = 0;
        }
    } else {
        eventEditor->clear();
        eventDisplayButton->setVisible(false);
        cornerButton->setIcon(QIcon()); // Убираем иконку
        cornerButton->setToolTip("");
        bellState = -1;
    }
    eventEditor->setBellState(bellState);
}

void CalendarCell::saveEventText() {
    QStringList events = eventEditor->getEventList();
    eventEditor->saveEventsToJson("events1.json");
    bellState = eventEditor->getBellState();
    if (!events.isEmpty()) {
        QStringList withBullets;
        for (const QString &text : events) {
            withBullets << "• " + text;
        }
        eventDisplayButton->setText(withBullets.join("\n"));
        eventDisplayButton->setVisible(true);
        addEventButton->setVisible(false);
        dayLabel->setVisible(true);

        // Сохраняем текущее состояние иконки, если оно есть
        if (bellState == 2) {
            cornerButton->setIcon(QIcon(":/new/prefix1/bell_2.png"));
            cornerButton->setToolTip("Удалить напоминание?");
        } else {
            cornerButton->setIcon(QIcon(":/new/prefix1/bell_0.png"));
            cornerButton->setToolTip("Напомнить о событии?");
            bellState = 0;
        }
    } else {
        eventEditor->clear();
        eventDisplayButton->setVisible(false);
        cornerButton->setIcon(QIcon());
        bellState = -1;
        if (this->underMouse()) {
            addEventButton->setVisible(true);
            dayLabel->setVisible(false);
        } else {
            dayLabel->setVisible(true);
        }
    }
    bellState = eventEditor->getBellState();
}

void CalendarCell::toggleBellIcon()
{
    QStringList events = eventEditor->getEventList();
    if (!events.isEmpty()) {
        if (bellState == 0) {
            cornerButton->setIcon(QIcon(":/new/prefix1/bell_2.png"));
            bellState = 2;
            cornerButton->setToolTip("Удалить напоминание?");  // Новая подсказка
        } else {
            cornerButton->setIcon(QIcon(":/new/prefix1/bell_0.png"));
            bellState = 0;
            cornerButton->setToolTip("Напомнить о событии?");  // Новая подсказка
        }
    }
    eventEditor->setBellState(bellState);
    eventEditor->saveEventsToJson("events1.json");
}


CalendarCell::~CalendarCell()
{
    delete ui;
}
