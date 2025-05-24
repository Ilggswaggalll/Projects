#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QPushButton>
#include <QMenu>
#include <QDate>
#include <QStackedWidget>
#include <QGestureEvent>
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

private:
    CalendarCell* currentEditingCell = nullptr;

public slots:
    void closeAllEventEditors();

private:
    CalendarCell* lastOpenedCell = nullptr;
private:
    Ui::MainWindow *ui;

    int selectedMonth = QDate::currentDate().month();
    int selectedYear = QDate::currentDate().year();

    QSpinBox *yearSpinBox;
    QPushButton *monthButton;
    QMenu *monthMenu;
    QPushButton *todayButton;

    void fillCalendar(int year, int month);
    void updateCalendar();

private:
    qreal accumulatedDelta = 0;
    const qreal swipeThreshold = 30.0;

private:
    QStackedWidget *monthStack;
    int currentMonthIndex;

private:
    QSystemTrayIcon *trayIcon;
    QTimer *notificationTimer;
    QDate lastNotificationDate;
    void checkForNotifications(bool forceCheck = false);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};


#endif // MAINWINDOW_H
