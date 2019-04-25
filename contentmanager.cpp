#include "pch.h"
#include "contentmanager.h"

ContentManager::ContentManager() : ContentManager(
    g_dataDir + "/content.txt")
{
}

ContentManager::ContentManager(const QString& filename) : m_filename(filename)
{
    QFile f(filename);
    f.open(QIODevice::ReadOnly);

    QTextStream ts(&f);
    m_content = ts.readAll();
    f.close();

    m_changed = false;
}

ContentManager::~ContentManager()
{
    saveIfNeeded();
}

void ContentManager::setContent(const QString &content)
{
    m_content = content;
    m_changed = true;
}

void ContentManager::saveIfNeeded()
{
    if (m_changed)
    {
        QFile f(m_filename);
        f.open(QIODevice::WriteOnly);
        QTextStream ts(&f);
        ts << m_content;
        f.close();

        m_changed = false;
    }
}

void ContentManager::backup()
{
    saveIfNeeded();
    QString bakFileName = m_filename + ".bak";
    QFile::remove(bakFileName);
    QFile::copy(m_filename, bakFileName);
}
