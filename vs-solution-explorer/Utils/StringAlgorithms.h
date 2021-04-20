#ifndef H_0EDA7C0F_1256_4983_A069_DF436207BEEA
#define H_0EDA7C0F_1256_4983_A069_DF436207BEEA

#include <list>
#include <string>
#include <vector>

std::list<std::string> getLines(const std::vector<char>& data, const std::size_t firstPosition = 0);

bool startWith(const std::string& string, const std::string& startString);

std::vector<std::string> split(const std::string& string, const std::string& delimeter);

/**
 * @brief Deletes whitespaces from the beginning and from the end
 * @return string without trailing whitespaces
 */
std::string trim(const std::string& string);

#endif // H_0EDA7C0F_1256_4983_A069_DF436207BEEA
