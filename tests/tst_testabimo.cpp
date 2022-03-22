#include <QtTest>

// add necessary includes here

class testAbimo : public QObject
{
    Q_OBJECT

public:
    testAbimo();
    ~testAbimo();

private slots:
    void test_case1();

};

testAbimo::testAbimo()
{

}

testAbimo::~testAbimo()
{

}

void testAbimo::test_case1()
{

}

QTEST_APPLESS_MAIN(testAbimo)

#include "tst_testabimo.moc"
