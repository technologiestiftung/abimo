#include <QDir>
#include <QFile>
#include <QtDebug>
#include <QtTest>
#include <QStringList>

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
};

testAbimo::testAbimo()
{

}

testAbimo::~testAbimo()
{

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
    QString db_file = "../../abimo/data/abimo_2019_mitstrassen.dbf"; // abimo_2012ges.dbf";

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
