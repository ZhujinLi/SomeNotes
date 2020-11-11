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

    void readsAndParsesContentFile() {
        // Make file
        QFile file(FILENAME);
        file.open(QIODevice::WriteOnly);
        QTextStream(&file) << "[\"hello, world\\nthis is the content\", \"the second note\"]";
        file.close();

        ContentManager mgr(FILENAME);
        QCOMPARE(mgr.getContentCount(), 2u);
        QCOMPARE(mgr.getContent(0)->getText(), "hello, world\nthis is the content");
        QCOMPARE(mgr.getContent(1)->getText(), "the second note");
    }

    void writesNotesToContentFile() {
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

    void handlesCRUD() {
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

    void generatesMessageForBrokenContentFile() {
        // Make file with invalid format
        QFile file(FILENAME);
        file.open(QIODevice::WriteOnly);
        QTextStream(&file) << "[\"hello, world\\nthis is the content\", \"the second note\"";
        file.close();

        ContentManager mgr(FILENAME);
        QCOMPARE(mgr.getContentCount(), 1u);
        QVERIFY(mgr.getContent(0)->getText().startsWith("File is broken"));
    }

    void handlesDeletion() {
        ContentManager *mgr = new ContentManager(FILENAME);

        NoteBlockContent *content = mgr->newContent();
        content->setText("some texts to be deleted");
        mgr->deleteContent(content);

        QFile delFile(DELFILENAME);
        delFile.open(QIODevice::ReadOnly);
        QTextStream ts(&delFile);
        const QString &s = ts.readAll();
        QVERIFY(s.contains("----"));
        QVERIFY(s.contains("Deleted"));
        QVERIFY(s.contains("some texts to be deleted"));
        delFile.close();

        delete mgr;
    }

    void putsNewlyDeletedContentOnTop() {
        ContentManager *mgr = new ContentManager(FILENAME);

        NoteBlockContent *content1 = mgr->newContent();
        content1->setText("first note");

        NoteBlockContent *content2 = mgr->newContent();
        content2->setText("second note");

        mgr->deleteContent(content1);
        mgr->deleteContent(content2);

        QFile delFile(DELFILENAME);
        delFile.open(QIODevice::ReadOnly);
        QTextStream ts(&delFile);
        const QString &s = ts.readAll();

        int pos1 = s.indexOf("first note");
        QVERIFY(pos1 != -1);

        int pos2 = s.indexOf("second note");
        QVERIFY(pos2 != -1);

        QVERIFY(pos2 < pos1);

        delFile.close();
        delete mgr;
    }
};

QTEST_MAIN(TstContentManager)

#include "tst_contentmanager.moc"
