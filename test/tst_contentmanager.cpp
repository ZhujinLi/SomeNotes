#include "../src/contentmanager.h"
#include "../src/noteblockcontent.h"
#include <QCoreApplication>
#include <QtTest>
#include <fstream>

static const char *FILENAME = "tst_content.txt";
static const char *DELFILENAME = "tst_content.txt.del";

class TstContentManager : public QObject {
    Q_OBJECT

private slots:
    void init() {
        remove(FILENAME);
        assert(!std::ifstream(FILENAME).good());

        remove(DELFILENAME);
        assert(!std::ifstream(DELFILENAME).good());
    }

    void cleanup() {
        remove(FILENAME);
        remove(DELFILENAME);
    }

    void test_case_read() {
        // Make file
        QFile file(FILENAME);
        file.open(QIODevice::WriteOnly);
        QTextStream ts(&file);
        ts << "[\"hello, world\\nthis is the content\", \"the second note\"]";
        file.close();

        ContentManager mgr(FILENAME);
        QCOMPARE(mgr.getContentCount(), 2u);
        QCOMPARE(mgr.getContent(0)->getText(), "hello, world\nthis is the content");
        QCOMPARE(mgr.getContent(1)->getText(), "the second note");
    }

    void test_case_write() {
        ContentManager *mgr = new ContentManager(FILENAME);
        QCOMPARE(mgr->getContentCount(), 0u);

        const QString text0 = "hello, world\nsecond line\n";
        auto content0 = mgr->newContent();
        content0->setText(text0);

        const QString text1 = "second record";
        auto content1 = mgr->newContent();
        content1->setText(text1);

        mgr->save();
        delete mgr;

        mgr = new ContentManager(FILENAME);
        QCOMPARE(mgr->getContentCount(), 2u);
        QCOMPARE(mgr->getContent(0)->getText(), text0);
        QCOMPARE(mgr->getContent(1)->getText(), text1);
        delete mgr;
    }

    void test_case_crud() {
        ContentManager *mgr = new ContentManager(FILENAME);
        QCOMPARE(mgr->getContentCount(), 0u);

        mgr->newContent()->setText("aaaa");
        mgr->newContent()->setText("bbbb");
        mgr->newContent()->setText("cccc");

        QCOMPARE(mgr->getContentCount(), 3u);
        QCOMPARE(mgr->getContent(0)->getText(), "aaaa");
        QCOMPARE(mgr->getContent(1)->getText(), "bbbb");
        QCOMPARE(mgr->getContent(2)->getText(), "cccc");

        mgr->getContent(1)->setText("BBBB");
        QCOMPARE(mgr->getContent(1)->getText(), "BBBB");

        QCOMPARE(mgr->deleteContent(nullptr), false);
        QCOMPARE(mgr->deleteContent(reinterpret_cast<NoteBlockContent *>(0x87654321)), false);
        NoteBlockContent *content1 = mgr->getContent(1);
        QCOMPARE(mgr->deleteContent(content1), true);
        QCOMPARE(mgr->deleteContent(content1), false);
        QCOMPARE(mgr->getContentCount(), 2u);
        QCOMPARE(mgr->getContent(0)->getText(), "aaaa");
        QCOMPARE(mgr->getContent(1)->getText(), "cccc");

        mgr->move(mgr->getContent(0), 1);
        QCOMPARE(mgr->getContent(0)->getText(), "cccc");
        QCOMPARE(mgr->getContent(1)->getText(), "aaaa");

        mgr->move(mgr->getContent(1), 0);
        QCOMPARE(mgr->getContent(0)->getText(), "aaaa");
        QCOMPARE(mgr->getContent(1)->getText(), "cccc");

        delete mgr;
    }

    void test_case_broken_file() {
        // Make file
        QFile file(FILENAME);
        file.open(QIODevice::WriteOnly);
        QTextStream ts(&file);
        ts << "[\"hello, world\\nthis is the content\", \"the second note\"";
        file.close();

        ContentManager mgr(FILENAME);
        QCOMPARE(mgr.getContentCount(), 1u);
        QVERIFY(mgr.getContent(0)->getText().startsWith("File is broken"));
    }

    void test_case_delete() {
        ContentManager *mgr = new ContentManager(FILENAME);

        NoteBlockContent *content = mgr->newContent();
        content->setText("first note");
        mgr->deleteContent(content);

        content = mgr->newContent();
        content->setText("second note");
        mgr->deleteContent(content);

        QFile delFile(DELFILENAME);
        delFile.open(QIODevice::ReadOnly);
        QTextStream ts(&delFile);
        const QString &s = ts.readAll();
        qInfo() << "File content: \n" << s;
        QVERIFY(s.contains("----"));
        QVERIFY(s.contains("Deleted"));
        QVERIFY(s.contains("first note"));
        QVERIFY(s.contains("second note"));
        delFile.close();

        delete mgr;
    }
};

QTEST_MAIN(TstContentManager)

#include "tst_contentmanager.moc"
