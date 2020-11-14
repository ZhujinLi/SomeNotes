#include "contentmanager.h"
#include "common.h"
#include "noteblockcontent.h"
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

ContentManager::ContentManager(const QString &fileName, const QString &trashFileName)
    : m_fileName(fileName), m_trashFileName(trashFileName), m_changeCount(0) {
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
        newContent()->setText("File is broken (may be disk damage or incompatible old format), raw content:\n" + text);
    }
}

ContentManager::~ContentManager() {
    save();

    for (NoteBlockContent *content : m_contents)
        delete content;
}

NoteBlockContent *ContentManager::newContent() {
    NoteBlockContent *content = new NoteBlockContent(this);
    m_contents.push_back(content);
    return content;
}

NoteBlockContent *ContentManager::_newContent(const QString &text) {
    NoteBlockContent *content = new NoteBlockContent(this, text);
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

    m_changeCount = 0;
    qInfo() << "Content trashed.";
}

bool ContentManager::trashContent(NoteBlockContent *content) {
    size_t index = _findIndex(content);
    if (index != SIZE_MAX) {
        const QString &text = content->getText();
        if (!text.isEmpty()) {
            _saveTextToTrash(text);
        }

        m_contents.erase(m_contents.begin() + static_cast<int>(index));
        delete content;
        save();
        return true;
    }
    return false;
}

void ContentManager::move(NoteBlockContent *content, int index) {
    size_t oldIndex = _findIndex(content);

    m_contents.erase(m_contents.begin() + static_cast<int>(oldIndex));
    m_contents.insert(m_contents.begin() + index, content);
}

void ContentManager::save() {
    QJsonArray arr;
    for (NoteBlockContent *content : m_contents) {
        arr.push_back(QJsonValue(content->getText()));
    }

    QJsonDocument doc(arr);

    QFile f(m_fileName);
    f.open(QIODevice::WriteOnly);
    QTextStream ts(&f);
    ts << doc.toJson();
    f.close();

    m_changeCount = 0;
    qInfo() << "Content saved.";
}

void ContentManager::notifyContentChange() {
    m_changeCount++;
    if (m_changeCount >= 20) {
        save();
    }
}

size_t ContentManager::_findIndex(NoteBlockContent *content) {
    for (size_t i = 0; i < m_contents.size(); i++)
        if (m_contents[i] == content) {
            return i;
        }
    return SIZE_MAX;
}
