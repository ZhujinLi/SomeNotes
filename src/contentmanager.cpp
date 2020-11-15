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
                _newContent(item.toString());
            }
        }
    }

    if (!parseSucc) {
        QString *content = newContent();
        *content = "File is broken (may be disk damage or incompatible old format), raw content:\n" + text;
    }
}

ContentManager::~ContentManager() {
    save();

    for (QString *content : m_contents)
        delete content;
}

QString *ContentManager::newContent() {
    QString *content = new QString();
    m_contents.push_back(content);
    return content;
}

QString *ContentManager::_newContent(const QString &text) {
    QString *content = new QString(text);
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

bool ContentManager::trashContent(QString *content) {
    size_t index = _findIndex(content);
    if (index != SIZE_MAX) {
        if (!content->isEmpty()) {
            _saveTextToTrash(*content);
        }

        m_contents.erase(m_contents.begin() + static_cast<int>(index));
        delete content;
        save();
        return true;
    }
    return false;
}

void ContentManager::move(QString *content, int index) {
    size_t oldIndex = _findIndex(content);

    m_contents.erase(m_contents.begin() + static_cast<int>(oldIndex));
    m_contents.insert(m_contents.begin() + index, content);
}

void ContentManager::save() {
    QJsonArray arr;
    for (QString *content : m_contents) {
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

size_t ContentManager::_findIndex(QString *content) {
    for (size_t i = 0; i < m_contents.size(); i++)
        if (m_contents[i] == content) {
            return i;
        }
    return SIZE_MAX;
}
