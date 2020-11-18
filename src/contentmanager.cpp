#include "contentmanager.h"
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

ContentManager::ContentManager(const QString &fileName, const QString &trashFileName)
    : m_fileName(fileName), m_trashFileName(trashFileName) {
    QFile f(fileName);
    f.open(QIODevice::ReadOnly);

    QTextStream ts(&f);
    QString text = ts.readAll();
    f.close();

    QJsonParseError err;
    QJsonDocument d = QJsonDocument::fromJson(text.toUtf8(), &err);

    bool parseSucc = true;
    if (text.isNull() || text.isEmpty())
        ;
    else if (err.error != QJsonParseError::NoError) {
        qCritical() << "Json error: " << err.errorString();
        parseSucc = false;
    } else {
        for (QJsonValueRef item : d.array()) {
            if (item.type() != QJsonValue::Type::String) {
                parseSucc = false;
                break;
            } else {
                QSharedPointer<QString> s(new QString(item.toString()));
                m_contents.push_back(s);
            }
        }
    }

    if (!parseSucc) {
        m_contents.push_back(QSharedPointer<QString>(
            new QString("File is broken (may be disk damage or incompatible old format), raw content:\n" + text)));
    }
}

ContentManager::~ContentManager() {
    save();
}

QSharedPointer<QString> ContentManager::newContent() {
    QSharedPointer<QString> content = QSharedPointer<QString>(new QString());
    m_contents.push_back(content);
    return content;
}

void ContentManager::_saveTextToTrash(const QString &text) {
    QFile f(m_trashFileName);
    f.open(QIODevice::ReadWrite);

    QByteArray fileContent = f.readAll();

    QString textWithLabel;
    QTextStream(&textWithLabel) << "-------- Trashed at " << QDateTime::currentDateTime().toString() << " --------\n"
                                << text << "\n";
    fileContent.insert(0, textWithLabel.toUtf8());

    f.resize(0);
    f.write(fileContent);
    f.close();

    qInfo() << "Content trashed.";
}

bool ContentManager::trashContent(QSharedPointer<QString> content) {
    int index = m_contents.indexOf(content);
    if (index != -1) {
        if (!content->isEmpty()) {
            _saveTextToTrash(*content);
        }

        m_contents.removeAt(index);
        save();
        return true;
    }
    return false;
}

void ContentManager::move(QSharedPointer<QString> content, int index) {
    int oldIndex = m_contents.indexOf(content);
    if (oldIndex == -1) {
        qCritical() << "Trying to move a content not existed!";
        return;
    }

    m_contents.erase(m_contents.begin() + oldIndex);
    m_contents.insert(m_contents.begin() + index, content);
}

void ContentManager::save() {
    QJsonArray arr;
    for (QSharedPointer<QString> content : m_contents) {
        arr.push_back(QJsonValue(*content));
    }

    QJsonDocument doc(arr);

    QFile f(m_fileName);
    f.open(QIODevice::WriteOnly);
    QTextStream ts(&f);
    ts << doc.toJson();
    f.close();

    qInfo() << "Content saved.";
}
