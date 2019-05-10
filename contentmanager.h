#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H
#include <QString>

class NoteBlockContent;

class ContentManager
{
public:
    ContentManager();
    ContentManager(const QString& filename);
    ~ContentManager();

    size_t getContentCount() { return m_contents.size(); }
    NoteBlockContent* getContent(size_t index) { return m_contents[index]; }

    NoteBlockContent* newContent();
    bool deleteContent(NoteBlockContent* content);

    void swap(NoteBlockContent* contentA, NoteBlockContent* contentB);

    void save();

    void backup();

    void notifyContentChange();

private:
    std::vector<NoteBlockContent*> m_contents;
    QString m_filename;
    int m_changeCount;

    size_t _findIndex(NoteBlockContent* content);
};

#endif // CONTENTMANAGER_H
