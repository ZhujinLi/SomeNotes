#include "contentmanager.h"

#include <QFile>
#include <QTextStream>

ContentManager::ContentManager(const QString& filename)
{
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        m_content = in.readAll();
        inputFile.close();
    }
}
