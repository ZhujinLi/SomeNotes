#ifndef NOTEBLOCKCONTENT_H
#define NOTEBLOCKCONTENT_H

#include <QString>

class ContentManager;

class NoteBlockContent {
public:
    explicit NoteBlockContent(ContentManager *mgr);
    explicit NoteBlockContent(ContentManager *mgr, const QString &text);

    void setText(const QString &text);
    const QString &getText() { return m_text; }

private:
    ContentManager *m_mgr;
    QString m_text;
};

#endif // NOTEBLOCKCONTENT_H
