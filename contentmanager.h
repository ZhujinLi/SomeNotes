#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H
#include <string>


class ContentManager
{
public:
    ContentManager(const std::string& filename = ":/data/content.txt");

    const std::string& getContent() { return m_content; }

private:
    std::string m_content;
};

#endif // CONTENTMANAGER_H
