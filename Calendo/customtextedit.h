// customtextedit.h
#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QTextEdit>
#include <QStringList>
#include <QDate>

class CustomTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit CustomTextEdit(QWidget *parent = nullptr);
    QStringList getEventList() const;
    void saveEventsToJson(const QString &filePath);
    void loadEventsFromJson(const QString &filePath);
    void setDate(const QDate &date);
    void showPlaceholder();

private:
    QDate currentDate;  // ✅ Переместить сюда
private:
    bool showingPlaceholder = false;
    QString placeholderText = "введите событие...";

signals:
    void enterPressed();

private:
    int bellState = -1;

public:
    void setBellState(int state);
    int getBellState() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setCursorToEnd();
};

#endif // CUSTOMTEXTEDIT_H
