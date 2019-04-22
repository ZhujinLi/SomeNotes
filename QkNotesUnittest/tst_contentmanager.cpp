#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <fstream>
#include "../contentmanager.h"

using namespace testing;

static const char* FILENAME = "tst_content.txt";

class TstContentManager : public Test
{
    // Test interface
protected:
    void SetUp();
    void TearDown();
};

void TstContentManager::SetUp()
{
    remove(FILENAME);
    assert(!std::ifstream(FILENAME).good());
}

void TstContentManager::TearDown()
{
    remove(FILENAME);
}

TEST_F(TstContentManager, read)
{
    const std::string content = "hello,world\nthis is the content";

    // Make file
    std::ofstream file(FILENAME);
    file << content;
    file.close();

    ContentManager mgr(FILENAME);
    EXPECT_STREQ(content.c_str(), mgr.getContent().c_str());
}

TEST_F(TstContentManager, noFile)
{
    ContentManager mgr("non_existent.txt");
    EXPECT_STREQ("", mgr.getContent().c_str());
}

TEST_F(TstContentManager, write)
{
    ContentManager* mgr = new ContentManager(FILENAME);
    ASSERT_STREQ("", mgr->getContent().c_str());

    const std::string content = "hello, world\nsecond line\n";
    mgr->setContent(content);
    mgr->saveIfNeeded();
    delete mgr;

    mgr = new ContentManager(FILENAME);
    EXPECT_STREQ(content.c_str(), mgr->getContent().c_str());
}
