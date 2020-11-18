#include "../src/contentmanager.h"
#include <QCoreApplication>
#include <QtTest>
#include <fstream>

static const char *FILE_NAME = "tst_content.txt";
static const char *TRASH_FILE_NAME = "tst_content.trash.txt";

class TstContentManager : public QObject {
    Q_OBJECT

private slots:
    void init() {
        remove(FILE_NAME);
        assert(!std::ifstream(FILE_NAME).good());

        remove(TRASH_FILE_NAME);
        assert(!std::ifstream(TRASH_FILE_NAME).good());
    }

    void cleanup() {
        remove(FILE_NAME);
        remove(TRASH_FILE_NAME);
    }

    void readsAndParsesContentFile() {
        // Make file
        QFile file(FILE_NAME);
        file.open(QIODevice::WriteOnly);
        QTextStream(&file) << "[\"hello, world\\nthis is the content\", \"the second note\"]";
        file.close();

        ContentManager mgr(FILE_NAME, TRASH_FILE_NAME);
        QCOMPARE(mgr.getContentCount(), 2u);
        QCOMPARE(*mgr.getContent(0), "hello, world\nthis is the content");
        QCOMPARE(*mgr.getContent(1), "the second note");
    }

    void writesNotesToContentFile() {
        ContentManager *mgr = new ContentManager(FILE_NAME, TRASH_FILE_NAME);
        QCOMPARE(mgr->getContentCount(), 0u);

        const QString text0 = "hello, world\nsecond line\n";
        auto content0 = mgr->newContent();
        *content0 = text0;

        const QString text1 = "second record";
        auto content1 = mgr->newContent();
        *content1 = text1;

        mgr->save();
        delete mgr;

        mgr = new ContentManager(FILE_NAME, TRASH_FILE_NAME);
        QCOMPARE(mgr->getContentCount(), 2u);
        QCOMPARE(*mgr->getContent(0), text0);
        QCOMPARE(*mgr->getContent(1), text1);
        delete mgr;
    }

    void handlesCRUD() {
        ContentManager *mgr = new ContentManager(FILE_NAME, TRASH_FILE_NAME);
        QCOMPARE(mgr->getContentCount(), 0u);

        *mgr->newContent() = "aaaa";
        *mgr->newContent() = "bbbb";
        *mgr->newContent() = "cccc";

        QCOMPARE(mgr->getContentCount(), 3u);
        QCOMPARE(*mgr->getContent(0), "aaaa");
        QCOMPARE(*mgr->getContent(1), "bbbb");
        QCOMPARE(*mgr->getContent(2), "cccc");

        *mgr->getContent(1) = "BBBB";
        QCOMPARE(*mgr->getContent(1), "BBBB");

        QCOMPARE(mgr->trashContent(QSharedPointer<QString>(new QString("xxx"))), false);
        QSharedPointer<QString> content1 = mgr->getContent(1);
        QCOMPARE(mgr->trashContent(content1), true);
        QCOMPARE(mgr->trashContent(content1), false);
        QCOMPARE(mgr->getContentCount(), 2u);
        QCOMPARE(*mgr->getContent(0), "aaaa");
        QCOMPARE(*mgr->getContent(1), "cccc");

        mgr->move(mgr->getContent(0), 1);
        QCOMPARE(*mgr->getContent(0), "cccc");
        QCOMPARE(*mgr->getContent(1), "aaaa");

        mgr->move(mgr->getContent(1), 0);
        QCOMPARE(*mgr->getContent(0), "aaaa");
        QCOMPARE(*mgr->getContent(1), "cccc");

        delete mgr;
    }

    void generatesMessageForBrokenContentFile() {
        // Make file with invalid format
        QFile file(FILE_NAME);
        file.open(QIODevice::WriteOnly);
        QTextStream(&file) << "[\"hello, world\\nthis is the content\", \"the second note\"";
        file.close();

        ContentManager mgr(FILE_NAME, TRASH_FILE_NAME);
        QCOMPARE(mgr.getContentCount(), 1u);
        QVERIFY(mgr.getContent(0)->startsWith("File is broken"));
    }

    void trashesContent() {
        ContentManager *mgr = new ContentManager(FILE_NAME, TRASH_FILE_NAME);

        QSharedPointer<QString> content = mgr->newContent();
        *content = "some texts to be trashed";
        mgr->trashContent(content);

        QFile trashFile(TRASH_FILE_NAME);
        trashFile.open(QIODevice::ReadOnly);
        QTextStream ts(&trashFile);
        const QString &s = ts.readAll();
        QVERIFY(s.contains("----"));
        QVERIFY(s.contains("Trashed"));
        QVERIFY(s.contains("some texts to be trashed"));
        trashFile.close();

        delete mgr;
    }

    void putsLastTrashedFirst() {
        ContentManager *mgr = new ContentManager(FILE_NAME, TRASH_FILE_NAME);

        QSharedPointer<QString> content1 = mgr->newContent();
        *content1 = "first note";

        QSharedPointer<QString> content2 = mgr->newContent();
        *content2 = "second note";

        mgr->trashContent(content1);
        mgr->trashContent(content2);

        QFile trashFile(TRASH_FILE_NAME);
        trashFile.open(QIODevice::ReadOnly);
        QTextStream ts(&trashFile);
        const QString &s = ts.readAll();

        int pos1 = s.indexOf("first note");
        QVERIFY(pos1 != -1);

        int pos2 = s.indexOf("second note");
        QVERIFY(pos2 != -1);

        QVERIFY(pos2 < pos1);

        trashFile.close();
        delete mgr;
    }

    void doesNotTrashEmptyContent() {
        ContentManager *mgr = new ContentManager(FILE_NAME, TRASH_FILE_NAME);

        QSharedPointer<QString> content1 = mgr->newContent();
        *content1 = "first note";
        mgr->trashContent(content1);

        QFile trashFile1(TRASH_FILE_NAME);
        trashFile1.open(QIODevice::ReadOnly);
        QString s1 = QTextStream(&trashFile1).readAll();
        trashFile1.close();

        QSharedPointer<QString> content2 = mgr->newContent();
        *content2 = "";
        mgr->trashContent(content2);

        QFile trashFile2(TRASH_FILE_NAME);
        trashFile2.open(QIODevice::ReadOnly);
        QString s2 = QTextStream(&trashFile2).readAll();
        trashFile2.close();

        QCOMPARE(s1, s2);

        delete mgr;
    }
};

QTEST_MAIN(TstContentManager)

#include "tst_contentmanager.moc"
