#include "TestSuite.h"

TestSuite::TestSuite()
{
    suite().push_back(this);
}

std::vector<QObject*>& TestSuite::suite()
{
    static std::vector<QObject*> objects;
    return objects;
}
