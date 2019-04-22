#include "contentmanager.h"
#include <fstream>
#include <sstream>


ContentManager::ContentManager(const std::string& filename) : m_filename(filename)
{
    std::ifstream inputFile(filename);
    std::stringstream ss;
    ss << inputFile.rdbuf();
    m_content = ss.str();

    m_changed = false;
}

ContentManager::~ContentManager()
{
    saveIfNeeded();
}

void ContentManager::setContent(const std::string &content)
{
    m_content = content;
    m_changed = true;
}

void ContentManager::saveIfNeeded()
{
    if (m_changed)
    {
        std::ofstream outputFile(m_filename);
        outputFile << m_content;

        m_changed = false;
    }
}
