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

    ui->calendarTable->setRowCount(6);  // Максимум 6 строк (недели)
    ui->calendarTable->setColumnCount(7);  // 7 дней в неделе

    //this->showMaximized();




    ui->calendarTable->clearContents();


    QStringList daysOfWeek = { "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс" };
    ui->calendarTable->setHorizontalHeaderLabels(daysOfWeek);



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





    ui->calendarTable->setRowCount(5);
    ui->calendarTable->setColumnCount(7);
    ui->calendarTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->calendarTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->calendarTable->horizontalHeader()->hide();
    ui->calendarTable->verticalHeader()->hide();



    ui->calendarTable->setStyleSheet(R"(
QTableWidget {
    background-color: #2f3136;
    color: white;
    gridline-color: #1e1f22;
    selection-background-color: #444;
    border: none;
    border-radius: 20px;
}
QHeaderView::section {
    background-color: #2f3136;
    color: white;
    font-weight: bold;
    border: none;
    border-bottom: 1px solid #1e1f22;
    padding: 8px;
    font-size: 14px;
    border-radius: 20px;
}
)");


}

MainWindow::~MainWindow()
{
    delete ui;
}
