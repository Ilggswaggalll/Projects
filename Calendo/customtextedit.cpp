#include "customtextedit.h"

#include <QKeyEvent>
#include <QStringList>
#include <QTextBlock>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>


CustomTextEdit::CustomTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    setPlainText("• ");
    setCursorToEnd();

    // Убираем полосы прокрутки полностью
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Устанавливаем стиль, который предотвращает появление прокрутки
    setStyleSheet(R"(
        QTextEdit {
            border: none;
            padding: 0;
            margin: 0;
        }
        QScrollBar:vertical {
            width: 0px;
        }
        QScrollBar:horizontal {
            height: 0px;
        }
    )");

    // Запрещаем изменение размера через Alt+колесо мыши
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void CustomTextEdit::keyPressEvent(QKeyEvent *event)
{
    QTextCursor cursor = textCursor();
    QString currentLine = cursor.block().text();
    int blockCount = document()->blockCount();


    // 🔒 Обработка удаления всего текста при полном выделении
    if ((event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) &&
        cursor.hasSelection())
    {
        int startBlock = cursor.selectionStart();
        int endBlock = cursor.selectionEnd();

        QTextCursor testCursor(document());
        testCursor.setPosition(startBlock);
        int startLine = testCursor.blockNumber();

        testCursor.setPosition(endBlock);
        int endLine = testCursor.blockNumber();

        if (startLine == 0 && endLine == blockCount - 1) {
            // Полный диапазон выбран, отменяем удаление и сбрасываем на "• "
            setPlainText("• ");
            setCursorToEnd();
            return;
        }
    }

    // Блокируем удаление первой строки, если в ней только "•"
    if ((event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) &&
        cursor.block().blockNumber() == 0 &&
        currentLine.trimmed() == "•")
    {
        return;
    }

    // Удаление всей строки, если курсор сразу после "• "
    if (event->key() == Qt::Key_Backspace &&
        cursor.positionInBlock() == 2 &&
        currentLine.startsWith("• "))
    {
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();
        cursor.deleteChar(); // удалить символ новой строки
        cursor.deletePreviousChar();
        return;
    }

    // Вставка новой строки
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (event->modifiers() & Qt::ShiftModifier) {
            cursor.insertText("\n• ");
        } else {
            emit enterPressed();
        }
        return;
    }

    // Передаём остальное в базовый класс
    QTextEdit::keyPressEvent(event);
}



void CustomTextEdit::setCursorToEnd()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}


void CustomTextEdit::saveEventsToJson(const QString &filePath) {
    QFile file(filePath);
    QJsonObject root;

    // Читаем старые данные
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isObject()) {
            root = doc.object();
        }
    }

    // Подготовим массив событий
    QStringList lines = this->toPlainText().split('\n');
    QJsonArray eventArray;
    for (const QString &line : lines) {
        QString text = line.trimmed();
        if (text.startsWith("• ")) {
            QString content = text.mid(2).trimmed();
            if (!content.isEmpty()) {
                QJsonObject obj;
                obj["text"] = content;
                eventArray.append(obj);
            }
        }

    }

    // Объединяем с состоянием колокольчика
    QJsonObject cellData;
    cellData["events"] = eventArray;
    cellData["bellState"] = bellState;


    root[currentDate.toString(Qt::ISODate)] = cellData;


    // Сохраняем обратно
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(root);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}


QStringList CustomTextEdit::getEventList() const {
    QStringList events;
    QTextDocument *doc = this->document();
    QTextBlock block = doc->firstBlock();

    while (block.isValid()) {
        QString text = block.text().trimmed();
        if (text.startsWith("• ")) {
            QString eventText = text.mid(2).trimmed(); // Убираем "• "
            if (!eventText.isEmpty()) {
                events << eventText;
            }
        }
        block = block.next();
    }
    return events;
}


void CustomTextEdit::loadEventsFromJson(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();
    QString key = currentDate.toString(Qt::ISODate);
    if (!root.contains(key)) {
        this->setPlainText("• ");
        setCursorToEnd();
        return;
    }

    //QJsonArray eventArray = root[key].toArray();

    QJsonObject cellData = root[key].toObject();
    QJsonArray eventArray = cellData["events"].toArray();
    bellState = cellData["bellState"].toInt(-1);

    QStringList lines;
    for (const QJsonValue &value : eventArray) {
        QString text = value.toObject()["text"].toString();
        lines << "• " + text;
    }

    if (lines.isEmpty()) {
        lines << "• ";
    }

    this->setPlainText(lines.join("\n"));
    setCursorToEnd();
}


void CustomTextEdit::setDate(const QDate &date) {
    currentDate = date;
}

void CustomTextEdit::setBellState(int state) {
    bellState = state;
}

int CustomTextEdit::getBellState() const {
    return bellState;
}
