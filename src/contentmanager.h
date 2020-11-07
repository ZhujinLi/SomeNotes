#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H
#include <QString>
#include <vector>

class NoteBlockContent;

class ContentManager {
public:
    ContentManager();
    ContentManager(const QString &filename);
    ~ContentManager();

    size_t getContentCount() { return m_contents.size(); }
    NoteBlockContent *getContent(size_t index) { return m_contents[index]; }

    NoteBlockContent *newContent();
    bool deleteContent(NoteBlockContent *content);

    void move(NoteBlockContent *content, int index);

    void save();

    void backup();

    void notifyContentChange();

private:
    std::vector<NoteBlockContent *> m_contents;
    QString m_filename;
    int m_changeCount;

    size_t _findIndex(NoteBlockContent *content);
    NoteBlockContent *_newContent(const QString &text);
    void _saveDeletedText(const QString &text);
};

#endif // CONTENTMANAGER_H
