#include "Utils/Bom.h"
#include "TestSuite.h"
#include <QtTest>

class BomTest: public TestSuite
{
    Q_OBJECT

public:
    BomTest();
    ~BomTest();

private slots:
    void test();
};

static BomTest BomTestObject;

BomTest::BomTest()
{
}

BomTest::~BomTest()
{
}

void BomTest::test()
{
    QCOMPARE(findBom({}), Bom::Unknown);

    QCOMPARE(findBom({'\xEF','\xBB','\xBF'}), Bom::Utf8);
    QCOMPARE(findBom({'\xFE','\xFF'}), Bom::Utf16Be);
    QCOMPARE(findBom({'\xFF','\xFE'}), Bom::Utf16Le);
    QCOMPARE(findBom({'\x00','\x00','\xFE','\xFF'}), Bom::Utf32Be);
    QCOMPARE(findBom({'\xFF','\xFE','\x00','\x00'}), Bom::Utf32Le);

    QCOMPARE(findBom({'\xEF','\xBB','\xBF','\r','\n'}), Bom::Utf8);
    QCOMPARE(findBom({'\xFE','\xFF','\r','\n'}), Bom::Utf16Be);
    QCOMPARE(findBom({'\xFF','\xFE','\r','\n'}), Bom::Utf16Le);
    QCOMPARE(findBom({'\x00','\x00','\xFE','\xFF','\r','\n'}), Bom::Utf32Be);
    QCOMPARE(findBom({'\xFF','\xFE','\x00','\x00','\r','\n'}), Bom::Utf32Le);

    QCOMPARE(findBom({'a','\xEF','\xBB','\xBF'}), Bom::Unknown);
    QCOMPARE(findBom({'a','\xFE','\xFF'}), Bom::Unknown);
    QCOMPARE(findBom({'a','\xFF','\xFE'}), Bom::Unknown);
    QCOMPARE(findBom({'a','\x00','\x00','\xFE','\xFF'}), Bom::Unknown);
    QCOMPARE(findBom({'a','\xFF','\xFE','\x00','\x00'}), Bom::Unknown);
}

#include "tst_utilsbom.moc"
