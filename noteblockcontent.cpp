#include "pch.h"
#include "noteblockcontent.h"
#include "contentmanager.h"

NoteBlockContent::NoteBlockContent(ContentManager* mgr) : m_mgr(mgr)
{
}

void NoteBlockContent::setText(const QString &text)
{
    m_text = text;
    m_mgr->notifyContentChange();
}
