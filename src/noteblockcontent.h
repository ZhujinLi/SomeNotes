#pragma once

#include <QString>

class ContentManager;

class NoteBlockContent {
public:
    NoteBlockContent() {}
    explicit NoteBlockContent(const QString &text) : m_text(text) {}

    void setText(const QString &text) { m_text = text; }
    const QString &getText() { return m_text; }

private:
    QString m_text;
};
