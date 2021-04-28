#include "Utils/RecentValues.h"
#include "TestSuite.h"
#include <QtTest>

using namespace std::string_literals;

class RecentValuesTest: public TestSuite
{
    Q_OBJECT

    struct TestContainer
    {
        bool isValid(const std::string& value)
        {
            return !value.empty();
        }

        std::string get(size_t i)
        {
            if (i < m_values.size()) {
                return m_values[i];
            }
            return {};
        }

        void set(size_t i, const std::string& value)
        {
            if (i < m_values.size()) {
                m_values[i] = value;
            }
        }

        std::vector<std::string> m_values;
    };

public:
    RecentValuesTest();
    ~RecentValuesTest();

    using Base = decltype (TestContainer::m_values);

    void test(const std::string& newValue, const Base& actualValues, Base expectedValues);

private slots:
    void test1();
};

static RecentValuesTest CircularBufferTestObject;

RecentValuesTest::RecentValuesTest()
{
}

RecentValuesTest::~RecentValuesTest()
{
}

void RecentValuesTest::test(const std::string& newValue,
                              const Base& actualValues, Base expectedValues)
{
    const int maxCount = 5;
    TestContainer container;
    container.m_values = actualValues;
    container.m_values.resize(maxCount);

    RecentValues<TestContainer, std::string> buffer{container, maxCount};
    buffer.addValue(newValue);

    expectedValues.resize(maxCount);
    QTRY_VERIFY(maxCount > 0);
    QCOMPARE(container.m_values, expectedValues);
}

void RecentValuesTest::test1()
{
    test("bbb", {"aaa", "bbb", "ccc"}, {"bbb", "aaa", "ccc"});
    test("bbb", {"bbb"}, {"bbb"});
    test("bbb", {"aaa"}, {"bbb", "aaa"});
    test("zzz", {"aaa", "bbb", "ccc", "ddd", "eee"},
                {"zzz", "aaa", "bbb", "ccc", "ddd"});
    test("aaa", {""}, {"aaa"});
    test("bbb", {"aaa", "bbb", "ccc", "ddd", "eee"},
                {"bbb", "aaa", "ccc", "ddd", "eee"});
}

#include "RecentValuesTest.moc"
