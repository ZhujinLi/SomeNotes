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

    NoteBlockContent* getContent() { return m_content; }

    void save();

    void backup();

private:
    NoteBlockContent* m_content;
    QString m_filename;
};

#endif // CONTENTMANAGER_H
