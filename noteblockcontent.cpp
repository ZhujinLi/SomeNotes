#include "pch.h"
#include "noteblockcontent.h"
#include "contentmanager.h"

NoteBlockContent::NoteBlockContent(ContentManager* mgr) : m_mgr(mgr)
{
    m_changed = false;
}

void NoteBlockContent::setText(const QString &text)
{
    m_text = text;
    m_changed = true;
}

void NoteBlockContent::saveIfNeeded()
{
    if (m_changed) {
        m_mgr->save();
        m_changed = false;
    }
}
