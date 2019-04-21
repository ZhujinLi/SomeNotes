#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H
#include "qstring.h"


class ContentManager
{
public:
    ContentManager(const QString& filename = ":/data/content.txt");

    const QString& getContent() { return m_content; }

private:
    QString m_content;
};

#endif // CONTENTMANAGER_H
