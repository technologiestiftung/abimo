#include <QDir>
#include <QFile>
#include <QtDebug>
#include <QString>
#include <QStringList>
#include <QtTest>

#include "../app/helpers.h"
#include "../app/dbaseReader.h"

class testAbimo : public QObject
{
    Q_OBJECT

public:
    testAbimo();
    ~testAbimo();

private slots:
    void test_helpers_containsAll();
    void test_helpers_filesAreIdentical();
    void test_requiredFields();
    void test_dbaseReader();
    QString testInputFile(QString fileName);
};

testAbimo::testAbimo() {}
testAbimo::~testAbimo() {}

QString testAbimo::testInputFile(QString fileName)
{
    QString filePath = QString("../../abimo/data/%1.dbf").arg(fileName);

    if (!QFile::exists(filePath)) {
        qDebug() << "File does not exist: " << filePath;
        qDebug() << "Current directory: " << QDir::currentPath();
        abort();
    }

    return filePath;
}

void testAbimo::test_helpers_containsAll()
{
    QHash<QString, int> hash;
    hash["one"] = 1;
    hash["two"] = 2;

    QCOMPARE(Helpers::containsAll(hash, {"one", "two"}), true);
    QCOMPARE(Helpers::containsAll(hash, {"one", "two", "three"}), false);    
}

void testAbimo::test_helpers_filesAreIdentical()
{
    QString file_1 = testInputFile("abimo_2019_mitstrassen");
    QString file_2 = testInputFile("abimo_2012ges");

    bool debug = false;

    QCOMPARE(Helpers::filesAreIdentical(file_1, file_1, debug), true);
    QCOMPARE(Helpers::filesAreIdentical(file_1, file_2, debug), false);
}

void testAbimo::test_requiredFields()
{
    QStringList strings = DbaseReader::requiredFields();
    QCOMPARE(strings.length(), 25L);
}

void testAbimo::test_dbaseReader()
{
    DbaseReader reader(testInputFile("abimo_2019_mitstrassen"));

    bool success = reader.checkAndRead();

    qDebug() << "fullError: " << reader.getFullError();

    QCOMPARE(success, true);
    QCOMPARE(reader.isAbimoFile(), true);
}

QTEST_APPLESS_MAIN(testAbimo)

#include "tst_testabimo.moc"
