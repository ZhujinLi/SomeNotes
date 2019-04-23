#include <QtTest>
#include <fstream>
#include "../contentmanager.h"

static const char* FILENAME = "tst_content.txt";

class TstContentManager : public QObject
{
    Q_OBJECT

public:
    TstContentManager();
    ~TstContentManager();

private slots:
    void init();
    void cleanup();

    void test_case_read();
    void test_case_no_file();
    void test_case_write();

};

TstContentManager::TstContentManager()
{

}

TstContentManager::~TstContentManager()
{

}

void TstContentManager::init()
{
    remove(FILENAME);
    assert(!std::ifstream(FILENAME).good());
}

void TstContentManager::cleanup()
{
    remove(FILENAME);
}

void TstContentManager::test_case_read()
{
    const std::string content = "hello,world\nthis is the content";

    // Make file
    std::ofstream file(FILENAME);
    file << content;
    file.close();

    ContentManager mgr(FILENAME);
    QCOMPARE(content.c_str(), mgr.getContent().c_str());
}

void TstContentManager::test_case_no_file()
{
    ContentManager mgr("non_existent.txt");
    QCOMPARE("", mgr.getContent().c_str());
}

void TstContentManager::test_case_write()
{
    ContentManager* mgr = new ContentManager(FILENAME);
    QCOMPARE("", mgr->getContent().c_str());

    const std::string content = "hello, world\nsecond line\n";
    mgr->setContent(content);
    mgr->saveIfNeeded();
    delete mgr;

    mgr = new ContentManager(FILENAME);
    QCOMPARE(content.c_str(), mgr->getContent().c_str());
}

QTEST_APPLESS_MAIN(TstContentManager)

#include "tst_contentmanager.moc"
