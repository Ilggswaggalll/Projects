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
    void setBellState(int state);
    int getBellState() const;

private:
    QDate currentDate;
    bool showingPlaceholder = false;
    int bellState = -1;
    void setCursorToEnd();

signals:
    void enterPressed();

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // CUSTOMTEXTEDIT_H
