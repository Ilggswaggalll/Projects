#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "calendarcell.h"

#include <QDate>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QToolTip>
#include <QApplication>
#include <QTouchEvent>
#include <QDebug>
#include <QHeaderView>
#include <QFrame>
#include <QApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QWidgetAction>
#include <QSystemTrayIcon>
#include <QThread>





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qApp->installEventFilter(this);

    QDate date = QDate::currentDate();
    selectedMonth = date.month();
    selectedYear = date.year();

    setAttribute(Qt::WA_AcceptTouchEvents);
    fillCalendar(selectedYear, selectedMonth);
    this->showMaximized();
    ui->centralwidget->setStyleSheet("QWidget#centralwidget { background-color: #1e1f22; }");

    QWidget *headerWidget = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 10, 20, 10);
    headerLayout->setSpacing(0);
    headerWidget->setStyleSheet("background-color: #2f3136; border-top-right-radius: 15px; border-top-left-radius: 15px");

    QStringList months = { "Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
                          "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь" };

    monthButton = new QPushButton(this);
    monthButton->setText(months[selectedMonth - 1]);

    monthMenu = new QMenu(this);
    for (int i = 0; i < months.size(); ++i) {
        QAction *action = monthMenu->addAction(months[i]);
        connect(action, &QAction::triggered, this, [=]() {
            selectedMonth = i + 1;
            monthButton->setText(months[i]);
            updateCalendar();
        });
    }

    monthMenu->setStyleSheet(R"(
QMenu {
    background-color: #2f3136;
    color: white;
    border: 1px solid #3a3b3f;
    padding: 6px;
    font-size: 16px;
    font-weight: bold;
    font-family: "Segoe UI";
    border-radius: 8px;
}
QMenu::item {
    padding: 6px 20px;
}
QMenu::item:selected {
    background-color: #3a3b3f;
    border-radius: 6px;
}
)");
    monthButton->setMenu(monthMenu);

    yearSpinBox = new QSpinBox(this);
    yearSpinBox->setRange(1900, 2100);
    yearSpinBox->setValue(selectedYear);
    yearSpinBox->setAlignment(Qt::AlignCenter);
    monthButton->setMinimumWidth(170);
    connect(yearSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateCalendar);

    monthButton->setStyleSheet(R"(
QPushButton {
    background-color: #1e1f22;
    color: white;
    font-size: 24px;
    font-weight: bold;
    border: none;
    border-top-left-radius: 15px;
    border-bottom-left-radius: 15px;
    padding: 6px 20px;
}
QPushButton::menu-indicator {
    image: none;
}
)");

    yearSpinBox->setStyleSheet(R"(
QSpinBox {
    background-color: #1e1f22;
    color: white;
    font-size: 24px;
    font-weight: bold;
    border: none;
    border-top-right-radius: 15px;
    border-bottom-right-radius: 15px;
    padding: 6px 2px;
    width: 100px;
    selection-background-color: transparent;
    selection-color: gray;
}
QSpinBox::up-button, QSpinBox::down-button {
    width: 0px;
}
)");

    QToolTip::setFont(QFont("Segoe UI", 10));
    qApp->setStyleSheet(R"(
QToolTip {
    background-color: #2f3136;
    color: white;
    border: 1px solid #5c5c5c;
    padding: 5px 5px;
    border-radius: 15px;
}
)");

    QFrame *innerDivider = new QFrame(this);
    innerDivider->setFrameShape(QFrame::VLine);
    innerDivider->setStyleSheet("color: #3a3b3f;");

    QFrame *dateSelectorFrame = new QFrame(this);
    dateSelectorFrame->setObjectName("dateSelectorFrame");
    QHBoxLayout *dateLayout = new QHBoxLayout(dateSelectorFrame);
    dateLayout->setContentsMargins(2, 2, 2, 2);
    dateLayout->setSpacing(0);
    //dateLayout->addWidget(monthButton);
    //dateLayout->addWidget(innerDivider);
    //dateLayout->addWidget(yearSpinBox);

    QPushButton *prevButton = new QPushButton(this);
    prevButton->setIcon(QIcon(":/new/prefix1/free-icon-left-arrow-271228.png"));
    prevButton->setIconSize(QSize(24, 24));
    prevButton->setFixedSize(40, 40);
    connect(prevButton, &QPushButton::clicked, this, [=]() {
        selectedMonth = (selectedMonth == 1) ? 12 : selectedMonth - 1;
        if (selectedMonth == 12) selectedYear--;
        monthButton->setText(months[selectedMonth - 1]);
        yearSpinBox->setValue(selectedYear);
        updateCalendar();
    });

    // Кнопка "вперёд"

    QPushButton *nextButton = new QPushButton(this);
    nextButton->setIcon(QIcon(":/new/prefix1/free-icon-right-arrow-271228.png"));
    nextButton->setIconSize(QSize(24, 24));
    nextButton->setFixedSize(40, 40);
    connect(nextButton, &QPushButton::clicked, this, [=]() {
        selectedMonth = (selectedMonth % 12) + 1;
        if (selectedMonth == 1) selectedYear++;
        monthButton->setText(months[selectedMonth - 1]);
        yearSpinBox->setValue(selectedYear);
        updateCalendar();
    });

    // Вставляем в layout
    dateLayout->addWidget(monthButton);

    dateLayout->addWidget(innerDivider);
    dateLayout->addWidget(yearSpinBox);

    dateSelectorFrame->setStyleSheet(R"(
#dateSelectorFrame {
    background-color: #1e1f22;
    border: 2px solid #3a3b3f;
    border-radius: 15px;
}
)");

    auto *mainLayout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
    if (mainLayout)
        mainLayout->insertWidget(0, headerWidget);

    ui->calendarTable->setRowCount(5);
    ui->calendarTable->setColumnCount(7);
    ui->calendarTable->horizontalHeader()->setHighlightSections(false);
    ui->calendarTable->verticalHeader()->hide();
    ui->calendarTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->calendarTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->calendarTable->setStyleSheet(R"(
QTableWidget {
    background-color: #2f3136;
    color: white;
    gridline-color: #1e1f22;
    border: none;
    border-radius: 20px;
    selection-background-color: transparent;
}
QHeaderView::section {
    background-color: #2f3136;
    color: white;
    font-weight: bold;
    font-size: 14px;
}
)");

    todayButton = new QPushButton("Сегодня", this);
    todayButton->setStyleSheet(R"(
QPushButton {
    background-color: #3a3b3f;
    color: white;
    font-size: 16px;
    border-radius: 10px;
    padding: 6px 12px;
}
QPushButton:hover {
    background-color: #505357;
}
)");
    todayButton->hide();

    connect(todayButton, &QPushButton::clicked, this, [=]() {
        QDate today = QDate::currentDate();
        selectedYear = today.year();
        selectedMonth = today.month();
        yearSpinBox->setValue(selectedYear);
        monthButton->setText(months[selectedMonth - 1]);
        todayButton->hide();
        fillCalendar(selectedYear, selectedMonth);
    });

    QWidget *leftSpacer = new QWidget(this);
    leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QWidget *rightContainer = new QWidget(this);
    QHBoxLayout *rightLayout = new QHBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->addStretch();
    rightLayout->addWidget(todayButton);

    headerLayout->addWidget(leftSpacer);
    headerLayout->addWidget(prevButton);
    headerLayout->addWidget(dateSelectorFrame);
    headerLayout->addWidget(nextButton);
    headerLayout->addWidget(rightContainer);

    monthButton->setToolTip("Выбрать месяц");
    yearSpinBox->setToolTip("Выбрать год");

    prevButton->setToolTip("Предыдущий месяц (или ← на клавиатуре)");
    nextButton->setToolTip("Следующий месяц (или → на клавиатуре)");

    todayButton->setToolTip("Вернуться к текущей дате");



    // Инициализация системного трея
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/new/prefix1/bell_2.png"));
    if (!trayIcon->isSystemTrayAvailable()) {
        qDebug() << "Системный трей не доступен!";
    }


    trayIcon->show();

    // Таймер для проверки времени (проверяем каждую минуту)
    notificationTimer = new QTimer(this);
    connect(notificationTimer, &QTimer::timeout, this, [this]() {
        checkForNotifications();
    });
    notificationTimer->start(60000); // 60 секунд

    // Первая проверка сразу при запуске (принудительно)
    checkForNotifications(true);


    // Создаем меню
    QMenu *trayMenu = new QMenu(this);

    // Добавляем текст (не кликабельный)
    QAction *statusAction = new QAction("Приложение запущено и готово напоминать о событиях", this);
    //statusAction->setEnabled(false); // Делаем неактивным (просто текст)

    // Добавляем разделитель и кнопки (если нужно)
    //trayMenu->addAction(statusAction);

    QLabel *label = new QLabel("Приложение запущено и готово напоминать о событиях");
    label->setMargin(5); // Отступы для красоты
    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(label);
    trayMenu->addAction(widgetAction);

    // Устанавливаем меню для иконки
    trayIcon->setContextMenu(trayMenu);
}




bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QStringList months = { "Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
                          "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь" };
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Left) {
            // Стрелка влево — предыдущий месяц
            selectedMonth = (selectedMonth == 1) ? 12 : selectedMonth - 1;
            if (selectedMonth == 12) selectedYear--;
            monthButton->setText(months[selectedMonth - 1]);
            yearSpinBox->setValue(selectedYear);
            updateCalendar();
            return true;
        } else if (keyEvent->key() == Qt::Key_Right) {
            // Стрелка вправо — следующий месяц
            selectedMonth = (selectedMonth % 12) + 1;
            if (selectedMonth == 1) selectedYear++;
            monthButton->setText(months[selectedMonth - 1]);
            yearSpinBox->setValue(selectedYear);
            updateCalendar();
            return true;
        }
    }

    // Передаём событие дальше
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::fillCalendar(int year, int month) {
    QDate firstDay(year, month, 1);
    int daysInMonth = firstDay.daysInMonth();
    int startDay = (firstDay.dayOfWeek() + 6) % 7;

    int totalCells = startDay + daysInMonth;
    int rows = (totalCells <= 35) ? 5 : 6;


    grabGesture(Qt::SwipeGesture);
    grabGesture(Qt::PanGesture);
    setAttribute(Qt::WA_AcceptTouchEvents);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, true);

    // Обработка свайпов
    grabGesture(Qt::SwipeGesture);

    ui->calendarTable->setRowCount(rows);
    ui->calendarTable->setColumnCount(7);
    ui->calendarTable->clearContents();

    QStringList daysOfWeek = { "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс" };
    ui->calendarTable->setHorizontalHeaderLabels(daysOfWeek);

    QDate prevMonth = firstDay.addMonths(-1);
    int prevDays = prevMonth.daysInMonth();
    int day = 1;
    int nextDay = 1;
    bool isCurrentMonthDay = false;

    for (int row = 0; row < 6; ++row) {
        for (int col = 0; col < 7; ++col) {
            int index = row * 7 + col;
            QWidget *cellWidget = nullptr;

            if (index < startDay) {
                int dayNumber = prevDays - (startDay - index) + 1;
                isCurrentMonthDay = false;
                cellWidget = new CalendarCell(prevMonth.year(), prevMonth.month(), dayNumber, isCurrentMonthDay);
                cellWidget->setEnabled(false);
                cellWidget->setStyleSheet("color: gray;");
            } else if (day <= daysInMonth) {
                isCurrentMonthDay = true;
                cellWidget = new CalendarCell(year, month, day++, isCurrentMonthDay);
            } else {
                QDate nextMonth = firstDay.addMonths(1);
                isCurrentMonthDay = false;
                cellWidget = new CalendarCell(nextMonth.year(), nextMonth.month(), nextDay++, isCurrentMonthDay);
                cellWidget->setEnabled(false);
                cellWidget->setStyleSheet("color: gray;");
            }

            // Подключаем сигнал каждой ячейки к слоту MainWindow для скрытия других редакторов
            CalendarCell* calendarCell = qobject_cast<CalendarCell*>(cellWidget);
            if (calendarCell) {
                connect(calendarCell, &CalendarCell::closeEventEditor, this, &MainWindow::closeAllEventEditors);

            }

            ui->calendarTable->setCellWidget(row, col, cellWidget);
        }
    }
}

void MainWindow::closeAllEventEditors() {
    // Перебираем все ячейки и скрываем их QTextEdit
    for (int row = 0; row < ui->calendarTable->rowCount(); ++row) {
        for (int col = 0; col < ui->calendarTable->columnCount(); ++col) {
            QWidget *widget = ui->calendarTable->cellWidget(row, col);
            CalendarCell* calendarCell = qobject_cast<CalendarCell*>(widget);
            if (calendarCell && calendarCell->isEventEditorVisible()) {
                calendarCell->saveEventText();
                calendarCell->hideEventEditor();
            }
        }
    }
}

void MainWindow::updateCalendar() {
    selectedYear = yearSpinBox->value();
    QDate date = QDate::currentDate();
    if (selectedYear == date.year() && selectedMonth == date.month()) {
        todayButton->hide();
    } else {
        todayButton->show();
    }
    fillCalendar(selectedYear, selectedMonth);
}


// метод проверки уведомлений
void MainWindow::checkForNotifications(bool forceCheck)
{
    QDate today = QDate::currentDate();
    QTime currentTime = QTime::currentTime();

    bool shouldCheck = forceCheck ||
                       (currentTime.hour() == 0 && currentTime.minute() == 0);

    if (shouldCheck && lastNotificationDate != today) {
        lastNotificationDate = today;

        for (int row = 0; row < ui->calendarTable->rowCount(); ++row) {
            for (int col = 0; col < ui->calendarTable->columnCount(); ++col) {
                QWidget *widget = ui->calendarTable->cellWidget(row, col);
                CalendarCell* calendarCell = qobject_cast<CalendarCell*>(widget);

                if (calendarCell && calendarCell->getBellState() == 2) {
                    QDate cellDate(calendarCell->getYear(),
                                   calendarCell->getMonth(),
                                   calendarCell->getDay());

                    if (cellDate == today) {
                        QStringList events = calendarCell->getEventList();
                        if (!events.isEmpty()) {
                            // Уведомление без автоскрытия
                            trayIcon->showMessage(
                                "Напоминание",
                                "Сегодня: " + events.join("\n"),
                                QSystemTrayIcon::Information,
                                0 // Уведомление останется пока пользователь не закроет
                                );
                        }
                    }
                }
            }
        }
    }
}


MainWindow::~MainWindow() {
    delete ui;
}



