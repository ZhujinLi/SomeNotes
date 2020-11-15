#pragma once

#include <QString>
#include <vector>

class NoteBlockContent;

class ContentManager {
public:
    ContentManager(const QString &fileName, const QString &trashFileName);
    ~ContentManager();

    size_t getContentCount() { return m_contents.size(); }
    NoteBlockContent *getContent(size_t index) { return m_contents[index]; }

    NoteBlockContent *newContent();
    bool trashContent(NoteBlockContent *content);

    void move(NoteBlockContent *content, int index);

    void save();

private:
    std::vector<NoteBlockContent *> m_contents;
    QString m_fileName;
    QString m_trashFileName;

    size_t _findIndex(NoteBlockContent *content);
    NoteBlockContent *_newContent(const QString &text);
    void _saveTextToTrash(const QString &text);
};
