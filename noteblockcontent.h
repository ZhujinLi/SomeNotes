#ifndef NOTEBLOCKCONTENT_H
#define NOTEBLOCKCONTENT_H

class ContentManager;

class NoteBlockContent
{
public:
    NoteBlockContent(ContentManager* mgr);

    void setText(const QString& text);
    const QString& getText() { return m_text; }

    void saveIfNeeded();

private:
    ContentManager* m_mgr;
    bool m_changed;
    QString m_text;
};

#endif // NOTEBLOCKCONTENT_H
