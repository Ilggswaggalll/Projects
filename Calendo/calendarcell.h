#ifndef CALENDARCELL_H
#define CALENDARCELL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QTextEdit>
#include <QGraphicsBlurEffect>
#include <QPushButton>
#include "customtextedit.h"
#include <QTextBrowser>



namespace Ui {
class CalendarCell;
}

class CalendarCell : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarCell(int year, int month, int day, bool isCurrentMonthDay, QWidget *parent = nullptr);
    ~CalendarCell();
    void saveEvent();
    bool isEditing() const { return eventInProgress && eventTextEdit->isVisible(); }
    void startEditing();
    bool isEventEditorVisible() const;  // Объявление метода для проверки видимости редактора
    bool isEventEditorModified() const;
    void hideEventEditor();
    QDate getDate() const;
    CustomTextEdit* getEditor() const;
    void saveEventText();
    QLabel *hintLabel;
    QPushButton *cornerButton;
    QString currentIconPath;
    int getYear() const { return year; }
    int getMonth() const { return month; }
    int getDay() const { return day; }
    int getBellState() const { return bellState; }
    QStringList getEventList() const { return eventEditor->getEventList(); }


protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void toggleBellIcon();
    void showEventEditor();

signals:
    void addEventClicked(int year, int month, int day);
    void closeEventEditor();

private:
    Ui::CalendarCell *ui;
    QString hoverBackground;
    bool isToday;
    bool isCurrentMonthDay = true; // новое поле
    int year;
    int month;
    int day;
    void updateBackground(bool hovered = false);
    QTextEdit *eventTextEdit = nullptr;
    bool eventInProgress = false;
    void onAddEventLabelClicked();
    int bellState; // 0 - bell_0, 2 - bell_2
    QVBoxLayout *layout;
    //QGridLayout *layout;
    QLabel *dayLabel;
    QPushButton *addEventButton;
    QString normalBackground;
    QPushButton* eventDisplayButton;
    CustomTextEdit *eventEdit;
    QPropertyAnimation *expandAnimation;
    QGraphicsBlurEffect *blurEffect;
    QWidget *blurOverlay;
    bool isExpanded = false;
    static bool isAnyCellEditing;
    CustomTextEdit *eventEditor;
};

#endif // CALENDARCELL_H
