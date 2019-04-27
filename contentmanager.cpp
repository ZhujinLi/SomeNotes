#include "pch.h"
#include "contentmanager.h"
#include "noteblockcontent.h"

ContentManager::ContentManager() : ContentManager(
    g_dataDir + "/contents.txt")
{
}

ContentManager::ContentManager(const QString& filename) : m_filename(filename)
{
    QFile f(filename);
    f.open(QIODevice::ReadOnly);

    m_content = new NoteBlockContent(this);

    QTextStream ts(&f);
    const QString& text = ts.readAll();
    m_content->setText(text);
    f.close();
}

ContentManager::~ContentManager()
{
    delete m_content;
    save();
}

void ContentManager::save()
{
    QFile f(m_filename);
    f.open(QIODevice::WriteOnly);
    QTextStream ts(&f);
    ts << m_content->getText();
    f.close();

    qInfo() << "Content saved.";
}

void ContentManager::backup()
{
    save();

    QString bakFileName = m_filename + ".bak";
    QFile::remove(bakFileName);
    QFile::copy(m_filename, bakFileName);
}
