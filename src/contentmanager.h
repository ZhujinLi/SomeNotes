#pragma once

#include <QSharedPointer>
#include <QString>
#include <vector>

class ContentManager {
public:
    /// @param fileName The file path for current note contents.
    /// @param trashFileName The file path for trashed contents.
    ContentManager(const QString &fileName, const QString &trashFileName);

    ~ContentManager();

    size_t getContentCount() { return m_contents.size(); }

    QSharedPointer<QString> getContent(size_t index) { return QSharedPointer<QString>(m_contents[index]); }

    QSharedPointer<QString> newContent();

    /// Remove the content and save it to the trash file.
    bool trashContent(QSharedPointer<QString> content);

    /// Move @p content from its original position to [index], while keeping other elements' order.
    void move(QSharedPointer<QString> content, int index);

    /// Save all the contents to disk.
    void save();

private:
    QVector<QSharedPointer<QString>> m_contents;
    QString m_fileName;
    QString m_trashFileName;

    void _saveTextToTrash(const QString &text);
};
