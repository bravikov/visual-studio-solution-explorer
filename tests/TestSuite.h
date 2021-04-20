#ifndef H_D500894C_3AD5_4857_864A_C11C513B6D8C
#define H_D500894C_3AD5_4857_864A_C11C513B6D8C

// https://alexhuszagh.github.io/2016/using-qttest-effectively/

#include <QObject>
#include <vector>

class TestSuite: public QObject
{
public:
     TestSuite();
     static std::vector<QObject*>& suite();
};

#endif // H_D500894C_3AD5_4857_864A_C11C513B6D8C
