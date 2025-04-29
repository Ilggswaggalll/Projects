#ifndef CALENDARCELL_H
#define CALENDARCELL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPropertyAnimation>


namespace Ui {
class CalendarCell;
}

class CalendarCell : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarCell(int year, int month, int day, QWidget *parent = nullptr);
    ~CalendarCell();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;


private:
    QVBoxLayout *layout;
    QLabel *dayLabel;
    QLabel *addEventLabel;
    QString normalBackground;     // Цвет в обычном состоянии

private:
    Ui::CalendarCell *ui;
    QString hoverBackground;
    bool isToday;
    void updateBackground(bool hovered = false);
};

#endif // CALENDARCELL_H
