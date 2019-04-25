#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H
#include <QString>


class ContentManager
{
public:
    ContentManager();
    ContentManager(const QString& filename);
    ~ContentManager();

    const QString& getContent() { return m_content; }

    void setContent(const QString& content);

    void saveIfNeeded();

    void backup();

private:
    QString m_content;
    bool m_changed;
    QString m_filename;
};

#endif // CONTENTMANAGER_H
