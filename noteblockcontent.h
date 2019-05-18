#ifndef NOTEBLOCKCONTENT_H
#define NOTEBLOCKCONTENT_H

class ContentManager;

class NoteBlockContent
{
public:
    NoteBlockContent(ContentManager* mgr);
    NoteBlockContent(ContentManager* mgr, const QString& text);

    void setText(const QString& text);
    const QString& getText() { return m_text; }

private:
    ContentManager* m_mgr;
    QString m_text;
};

#endif // NOTEBLOCKCONTENT_H
