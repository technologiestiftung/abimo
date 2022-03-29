#include <QDir>
#include <QFile>
#include <QtDebug>
#include <QString>
#include <QStringList>
#include <QtTest>

#include "../app/calculation.h"
#include "../app/dbaseReader.h"
#include "../app/helpers.h"

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
    void test_calc();

    QString testDataDir();
    QString dataFilePath(QString fileName, bool mustExist = true);
    void abortIfNoSuchFile(QString filePath);
};

testAbimo::testAbimo() {}
testAbimo::~testAbimo() {}

void testAbimo::abortIfNoSuchFile(QString filePath)
{
    if (!QFile::exists(filePath)) {
        qDebug() << "File does not exist: " << filePath;
        qDebug() << "Current directory: " << QDir::currentPath();
        qDebug() << "Data directory: " << testDataDir();
        abort();
    }
}

QString testAbimo::testDataDir()
{
    return QString("../../../abimo/data");
}

QString testAbimo::dataFilePath(QString fileName, bool mustExist)
{
    QString filePath = QString(testDataDir() + "/" + fileName);

    if (mustExist) {
        abortIfNoSuchFile(filePath);
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
    QString file_1 = dataFilePath("abimo_2019_mitstrassen.dbf");
    QString file_2 = dataFilePath("abimo_2012ges.dbf");

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
    DbaseReader reader(dataFilePath("abimo_2019_mitstrassen.dbf"));

    bool success = reader.checkAndRead();

    qDebug() << "fullError: " << reader.getFullError();

    QCOMPARE(success, true);
    QCOMPARE(reader.isAbimoFile(), true);
}

void testAbimo::test_calc()
{
    QString inputFile = dataFilePath("abimo_2019_mitstrassen.dbf");
    QString configFile = dataFilePath("config.xml");
    QString outputFile = dataFilePath("tmp_out.dbf", false);
    QString referenceFile = dataFilePath("abimo_2019_mitstrassen_out.dbf");

    Calculation::calculate(inputFile, "" /*configFile*/, outputFile);

    QCOMPARE(Helpers::filesAreIdentical(outputFile, referenceFile), true);
}

QTEST_APPLESS_MAIN(testAbimo)

#include "tst_testabimo.moc"
