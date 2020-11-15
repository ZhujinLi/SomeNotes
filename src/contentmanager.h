#pragma once

#include <QString>
#include <vector>

class ContentManager {
public:
    ContentManager(const QString &fileName, const QString &trashFileName);
    ~ContentManager();

    size_t getContentCount() { return m_contents.size(); }
    QString *getContent(size_t index) { return m_contents[index]; }

    QString *newContent();
    bool trashContent(QString *content);

    void move(QString *content, int index);

    void save();

private:
    std::vector<QString *> m_contents;
    QString m_fileName;
    QString m_trashFileName;

    size_t _findIndex(QString *content);
    QString *_newContent(const QString &text);
    void _saveTextToTrash(const QString &text);
};
