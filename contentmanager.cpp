#include "pch.h"
#include "contentmanager.h"
#include "noteblockcontent.h"

#include <QJsonArray>
#include <QJsonDocument>

ContentManager::ContentManager() : ContentManager(
    g_dataDir + "/content.txt")
{
}

ContentManager::ContentManager(const QString& filename) : m_filename(filename)
{
    QFile f(filename);
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
                newContent()->setText(item.toString());
            }
        }
    }

    if (!parseSucc) {
        newContent()->setText(
            "File is broken (may be disk damage or incompatible old format), raw content:\n" + text);
    }
}

ContentManager::~ContentManager()
{
    save();

    for (NoteBlockContent* content : m_contents)
        delete content;
}

NoteBlockContent *ContentManager::newContent()
{
    NoteBlockContent* content = new NoteBlockContent(this);
    m_contents.push_back(content);
    return content;
}

bool ContentManager::deleteContent(NoteBlockContent *content)
{
    for (size_t i = 0; i < m_contents.size(); i++)
        if (m_contents[i] == content) {
            m_contents.erase((m_contents.begin() + static_cast<long long>(i)));
            delete content;
            return true;
        }
    return false;
}



void ContentManager::save()
{
    QJsonArray arr;
    for (NoteBlockContent* content : m_contents) {
        arr.push_back(QJsonValue(content->getText()));
    }

    QJsonDocument doc(arr);

    QFile f(m_filename);
    f.open(QIODevice::WriteOnly);
    QTextStream ts(&f);
    ts << doc.toJson();
    f.close();

    qInfo() << "Content saved.";
}

void ContentManager::backup()
{
    save();

    QString bakFileName = m_filename + ".bak";
    QFile::remove(bakFileName);
    QFile::copy(m_filename, bakFileName);
}
