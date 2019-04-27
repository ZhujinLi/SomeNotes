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

    void save();

    void backup();

private:
    std::vector<NoteBlockContent*> m_contents;
    QString m_filename;
};

#endif // CONTENTMANAGER_H
