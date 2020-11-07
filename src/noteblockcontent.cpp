#include "noteblockcontent.h"
#include "contentmanager.h"
#include "pch.h"

NoteBlockContent::NoteBlockContent(ContentManager *mgr) : m_mgr(mgr) {}

NoteBlockContent::NoteBlockContent(ContentManager *mgr, const QString &text) : m_mgr(mgr), m_text(text) {}

void NoteBlockContent::setText(const QString &text) {
    if (text != m_text) {
        m_text = text;
        m_mgr->notifyContentChange();
    }
}
