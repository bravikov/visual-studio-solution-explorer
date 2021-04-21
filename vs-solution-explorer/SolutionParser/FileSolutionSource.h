#ifndef H_E71EFC7E_8CB8_4D5E_94D7_00B261F6A96B
#define H_E71EFC7E_8CB8_4D5E_94D7_00B261F6A96B

#include "ISolutionSource.h"
#include <string>

class FileSolutionSource: public ISolutionSource
{
public:
    explicit FileSolutionSource(const std::string& filename);
    const std::vector<char>& content() const override;

private:
    std::vector<char> m_content;
};

#endif // H_E71EFC7E_8CB8_4D5E_94D7_00B261F6A96B
