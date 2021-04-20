#include "Utils/StringAlgorithms.h"
#include "TestSuite.h"
#include <QtTest>

using namespace std::string_literals;

class StringAlgorithmTest: public TestSuite
{
    Q_OBJECT

public:
    StringAlgorithmTest();
    ~StringAlgorithmTest();

private slots:
    void getLinesTest();
    void getLinesWithOffsetTest();
    void startWithTest();
    void splitTest();
    void trimTest();

private:
    static std::vector<char> stringToVector(const std::string& string);

};

static StringAlgorithmTest StringAlgorithmTestObject;
StringAlgorithmTest::StringAlgorithmTest()
{
}

StringAlgorithmTest::~StringAlgorithmTest()
{
}

std::vector<char> StringAlgorithmTest::stringToVector(const std::string& string)
{
    return std::vector<char>{string.begin(), string.end()};
}

void StringAlgorithmTest::getLinesTest()
{
    QCOMPARE(getLines(stringToVector("")), {});
    QCOMPARE(getLines(stringToVector("aaa")), {"aaa"s});

    std::list<std::string> expected;

    // \n

    QCOMPARE(getLines(stringToVector("\n")), {""s});

    expected = {""s, ""s};
    QCOMPARE(getLines(stringToVector("\n\n")), expected);

    expected = {"aaa"s, "bbb"s};
    QCOMPARE(getLines(stringToVector("aaa\nbbb")), expected);

    expected = {"aaa"s, "bbb"s};
    QCOMPARE(getLines(stringToVector("aaa\nbbb\n")), expected);

    expected = {""s, "aaa"s, "bbb"s};
    QCOMPARE(getLines(stringToVector("\naaa\nbbb\n")), expected);

    // \r\n

    QCOMPARE(getLines(stringToVector("\r\n")), {""s});

    expected = {""s, ""s};
    QCOMPARE(getLines(stringToVector("\r\n\r\n")), expected);

    expected = {"aaa"s, "bbb"s};
    QCOMPARE(getLines(stringToVector("aaa\r\nbbb")), expected);

    expected = {"aaa"s, "bbb"s};
    QCOMPARE(getLines(stringToVector("aaa\r\nbbb\r\n")), expected);

    expected = {""s, "aaa"s, "bbb"s};
    QCOMPARE(getLines(stringToVector("\r\naaa\r\nbbb\r\n")), expected);

    // \r
    QCOMPARE(getLines(stringToVector("\r")), {"\r"s});
    QCOMPARE(getLines(stringToVector("\r\r")), {"\r\r"s});
    QCOMPARE(getLines(stringToVector("aaa\rbbb")), {"aaa\rbbb"});
    QCOMPARE(getLines(stringToVector("aaa\rbbb\r")), {"aaa\rbbb\r"});
    QCOMPARE(getLines(stringToVector("\raaa\rbbb\r")), {"\raaa\rbbb\r"});

    // \n and \r\n
    expected = {"aaa"s, "bbb"s, "ccc"s};
    QCOMPARE(getLines(stringToVector("aaa\nbbb\r\nccc")), expected);
    expected = {"aaa"s, "bbb"s, "ccc"s};
    QCOMPARE(getLines(stringToVector("aaa\r\nbbb\nccc")), expected);
}

void StringAlgorithmTest::getLinesWithOffsetTest()
{
    std::list<std::string> expected;

    expected = {"aaa"s, "bbb"s};
    QCOMPARE(getLines(stringToVector("cccaaa\r\nbbb"), 3), expected);
}

void StringAlgorithmTest::startWithTest()
{
    QVERIFY(!startWith("", ""));
    QVERIFY(!startWith("abc",""));
    QVERIFY(!startWith("","abc"));
    QVERIFY(!startWith("ab","abc"));
    QVERIFY(!startWith("abc","bc"));
    QVERIFY(!startWith("abcd","bc"));

    QVERIFY(startWith("abc","abc"));
    QVERIFY(startWith("abc","ab"));
}

void StringAlgorithmTest::splitTest()
{
    QCOMPARE(split("",""), {});
    QCOMPARE(split("aaa",""), {});
    QCOMPARE(split("","aaa"), {""});
    QCOMPARE(split("aaa","b"), {"aaa"});

    std::vector<std::string> expected;

    expected = {"aaa", "bbb"};
    QCOMPARE(split("aaa=bbb","="), expected);

    expected = {"aaa", "bbb", "ccc"};
    QCOMPARE(split("aaa=bbb=ccc","="), expected);

    expected = {"aaa", "bbb", "ccc"};
    QCOMPARE(split("aaa==bbb==ccc","=="), expected);

    expected = {"aaa", "bbb=ccc"};
    QCOMPARE(split("aaa==bbb=ccc","=="), expected);

    expected = {"", ""};
    QCOMPARE(split("=","="), expected);

    expected = {"aaa", "bbb", ""};
    QCOMPARE(split("aaa==bbb==","=="), expected);

    expected = {"", "aaa", "bbb", ""};
    QCOMPARE(split("==aaa==bbb==","=="), expected);
}

void StringAlgorithmTest::trimTest()
{
    QCOMPARE(trim(""), ""s);
    QCOMPARE(trim(" "), ""s);
    QCOMPARE(trim("  "), ""s);
    QCOMPARE(trim("   "), ""s);
    QCOMPARE(trim("    "), ""s);
    QCOMPARE(trim("aaa"), "aaa"s);
    QCOMPARE(trim(" aaa "), "aaa"s);
    QCOMPARE(trim("   aaa"), "aaa"s);
    QCOMPARE(trim("aaa   "), "aaa"s);
    QCOMPARE(trim(" aaa   "), "aaa"s);
    QCOMPARE(trim("   aaa   "), "aaa"s);
    QCOMPARE(trim("   aaa   bbb   "), "aaa   bbb"s);
}

#include "StringAlgorithmsTest.moc"
