#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H
#include <string>


class ContentManager
{
public:
    ContentManager(const std::string& filename = ":/data/content.txt");
    ~ContentManager();

    const std::string& getContent() { return m_content; }

    void setContent(const std::string& content);

    void saveIfNeeded();

private:
    std::string m_content;
    bool m_changed;
    std::string m_filename;
};

#endif // CONTENTMANAGER_H
