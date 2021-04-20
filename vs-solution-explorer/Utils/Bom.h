#ifndef H_B60E2CC8_3EB7_42BB_8488_367B2EA6ED0B
#define H_B60E2CC8_3EB7_42BB_8488_367B2EA6ED0B

#include <vector>

enum class Bom {
    Unknown,
    Utf8,
    Utf16Be,
    Utf16Le,
    Utf32Be,
    Utf32Le,
};

Bom findBom(const std::vector<char>& data);
std::size_t bomSize(Bom bom);

#endif // H_B60E2CC8_3EB7_42BB_8488_367B2EA6ED0B
