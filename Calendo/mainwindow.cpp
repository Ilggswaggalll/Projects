#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "calendarcell.h"

#include <QDate>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

void MainWindow::fillCalendar(int year, int month) {
    QDate firstDay(year, month, 1);


    int daysInMonth = firstDay.daysInMonth();
    int startDay = (firstDay.dayOfWeek() + 6) % 7; // Пн = 0

    ui->calendarTable->setRowCount(5);  // Максимум 6 строк (недели)
    ui->calendarTable->setColumnCount(7);  // 7 дней в неделе
    ui->calendarTable->clearContents();

    QStringList daysOfWeek = { "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс" };
    ui->calendarTable->setHorizontalHeaderLabels(daysOfWeek);

    //                if (day == today){ui->calendarTable-}

    int day = 1;
    for (int row = 0; row < 6; ++row) {
        for (int col = 0; col < 7; ++col) {
            int index = row * 7 + col;
            if (index >= startDay && day <= daysInMonth) {
                CalendarCell *cellWidget = new CalendarCell(year, month, day);
                ui->calendarTable->setCellWidget(row, col, cellWidget);
                day++;
            }
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fillCalendar(2025, 4);

    this->showMaximized();

    //ui->centralwidget->setContentsMargins(10, 10, 10, 10);

    // Контейнер над таблицей
    QWidget *headerWidget = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 10, 20, 10);
    headerLayout->setSpacing(15);
    headerWidget->setStyleSheet("background-color: #2f3136;");

    // Кнопка месяца
    monthButton = new QPushButton(this);
    QStringList months = { "Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
                          "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь" };
    monthButton->setText(months[selectedMonth - 1]);
    monthButton->setStyleSheet(R"(
    QPushButton {
        color: white;
        background-color: transparent;
        font-size: 24px;
        font-weight: bold;
        border: none;
        padding: 5px 10px;
    }
    QPushButton::menu-indicator { image: none; }
)");

    monthMenu = new QMenu(this);
    for (int i = 0; i < months.size(); ++i) {
        QAction *action = monthMenu->addAction(months[i]);
        connect(action, &QAction::triggered, this, [=]() {
            selectedMonth = i + 1;
            monthButton->setText(months[i]);
            fillCalendar(selectedYear, selectedMonth);
        });
    }
    monthButton->setMenu(monthMenu);

    // Спинбокс года
    yearSpinBox = new QSpinBox(this);
    yearSpinBox->setRange(1900, 2100);
    yearSpinBox->setValue(selectedYear);

    yearSpinBox->setStyleSheet(R"(
    QSpinBox {
        color: white;
        background-color: transparent;
        font-size: 24px;
        font-weight: bold;
        border: none;
        width: 100px;
        selection-background-color: transparent;
        selection-color: white;
    }
)");

    connect(yearSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int year){
        selectedYear = year;
        fillCalendar(selectedYear, selectedMonth);
    });

    // Вставляем в layout
    headerLayout->addStretch();
    headerLayout->addWidget(monthButton);
    headerLayout->addWidget(yearSpinBox);
    headerLayout->addStretch();

    auto *mainLayout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
    if (mainLayout)
        mainLayout->insertWidget(0, headerWidget);





    ui->calendarTable->setRowCount(5);
    ui->calendarTable->setColumnCount(7);
    ui->calendarTable->horizontalHeader()->setHighlightSections(false);
    ui->calendarTable->verticalHeader()->setHighlightSections(false);
    ui->calendarTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->calendarTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->calendarTable->verticalHeader()->hide();
    ui->calendarTable->setContentsMargins(0, 0, 0, 0);
    ui->calendarTable->setStyleSheet(R"(
QTableWidget {
    background-color: #2f3136;
    color: white;
    gridline-color: #1e1f22;
    selection-background-color: transparent;
    border: none;
    border-radius: 20px;
}
QHeaderView::section {
    background-color: #2f3136;
    color: white;
    font-weight: bold;
    padding: 8px;
    font-size: 14px;
    selection-background-color: transparent;
}
)");


}

MainWindow::~MainWindow()
{
    delete ui;
}
