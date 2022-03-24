#include <QDir>
#include <QException>
#include <QFile>
#include <QtDebug>
#include <QtTest>
#include <QStringList>

#include <stdexcept>

#include "../app/helpers.h"
#include "../app/dbaseReader.h"

// add necessary includes here

class testAbimo : public QObject
{
    Q_OBJECT

public:
    testAbimo();
    ~testAbimo();

private slots:
    void test_helpers();
    void test_dbaseReader();
    void openFileOrAbort(QFile *file);
    bool filesAreIdentical(QString file_1, QString file_2);
};

testAbimo::testAbimo()
{

}

testAbimo::~testAbimo()
{

}

void testAbimo::openFileOrAbort(QFile *file)
{
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file: " << file->fileName() << ": " << file->errorString();
        abort();
    }
}

bool testAbimo::filesAreIdentical(QString fileName_1, QString fileName_2)
{
    qDebug() << "Comparing two files: ";
    qDebug() << "File 1: " << fileName_1;
    qDebug() << "File 2: " << fileName_2;

    QFile file_1(fileName_1);
    QFile file_2(fileName_2);

    openFileOrAbort(& file_1);
    openFileOrAbort(& file_2);

    QByteArray blob_1 = file_1.readAll();
    QByteArray blob_2 = file_2.readAll();

    bool result = true;

    if (blob_1.length() != blob_2.length()) {
        result = false;
    }

    if (result) {
        for (int i = 0; i < blob_1.length(); i++) {
            if (blob_1[i] != blob_2[i]) {
                result = false;
            }
        }
    }

    qDebug() << "The files are " << (result ? "": "not ") << "identical.";
    return result;
}

void testAbimo::test_helpers()
{
    QHash<QString, int> hash;
    hash["one"] = 1;
    hash["two"] = 2;

    QCOMPARE(Helpers::containsAll(hash, {"one", "two"}), true);
    QCOMPARE(Helpers::containsAll(hash, {"one", "two", "three"}), false);
}

void testAbimo::test_dbaseReader()
{
    QString db_file = "../../abimo/data/abimo_2019_mitstrassen.dbf";
    QString db_file_2 = "../../abimo/data/abimo_2012ges.dbf";

    QCOMPARE(filesAreIdentical(db_file, db_file), true);
    QCOMPARE(filesAreIdentical(db_file, db_file_2), false);

    /*
    qDebug() << "Current directory: " << QDir::currentPath();
    qDebug() << "db_file exists? " << QFile::exists(db_file);
    */

    DbaseReader reader(db_file);
    QStringList strings = reader.requiredFields();
    QCOMPARE(strings.length(), 25L);

    bool success = reader.checkAndRead();

    qDebug() << "fullError: " << reader.getFullError();

    QCOMPARE(success, true);
    QCOMPARE(reader.isAbimoFile(), true);
}

QTEST_APPLESS_MAIN(testAbimo)

#include "tst_testabimo.moc"
