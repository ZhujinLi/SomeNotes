#include "contentmanager.h"
#include <fstream>
#include <sstream>


ContentManager::ContentManager(const std::string& filename)
{
    std::ifstream inputFile(filename);
    printf("hwllo\n");
    std::stringstream ss;
    ss << inputFile.rdbuf();
    m_content = ss.str();
}
