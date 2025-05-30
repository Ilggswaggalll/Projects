#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QPushButton>
#include <QMenu>
#include <QDate>
#include <QStackedWidget>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QTime>


class CalendarCell;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void closeAllEventEditors();

private:
    Ui::MainWindow *ui;

    CalendarCell* currentEditingCell = nullptr;
    CalendarCell* lastOpenedCell = nullptr;

    int selectedMonth = QDate::currentDate().month();
    int selectedYear = QDate::currentDate().year();

    QSpinBox *yearSpinBox;
    QPushButton *monthButton;
    QMenu *monthMenu;
    QPushButton *todayButton;

    void fillCalendar(int year, int month);
    void updateCalendar();

    qreal accumulatedDelta = 0;
    const qreal swipeThreshold = 30.0;


    QStackedWidget *monthStack;
    int currentMonthIndex;

    QSystemTrayIcon *trayIcon;
    QTimer *notificationTimer;
    QDate lastNotificationDate;
    void checkForNotifications(bool forceCheck = false);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};


#endif // MAINWINDOW_H
