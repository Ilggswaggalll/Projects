#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void fillCalendar(int year, int month);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QPushButton *monthButton;
    QMenu *monthMenu;
    QSpinBox *yearSpinBox;
    int selectedMonth = 4;
    int selectedYear = 2025;


private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
