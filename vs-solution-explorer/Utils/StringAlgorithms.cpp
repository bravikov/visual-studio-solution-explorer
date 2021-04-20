#include "StringAlgorithms.h"
#include <stdexcept>

std::list<std::string> getLines(const std::vector<char>& data, const std::size_t firstPosition)
{
    if (data.size() < firstPosition) {
        throw std::logic_error{"Small data"};
    }
    std::list<std::string> lines;
    auto it = data.begin();
    std::advance(it, firstPosition);
    auto lineBegin = it;
    auto lineEnd = lineBegin;
    for (; it != data.cend(); ++it) {
        const bool cr = (*it == '\r');
        const bool lf = (*it == '\n');
        if (cr) {
            continue;
        }
        else if (lf) {
            std::string newString{lineBegin, lineEnd};
            lines.push_back(newString);
            lineEnd = lineBegin = it + 1;
        }
        else {
            ++lineEnd;
        }
    }

    if (lineBegin != data.cend()) {
        std::string newString{lineBegin, data.cend()};
        lines.push_back(newString);
    }

    return lines;
}

bool startWith(const std::string& string, const std::string& startString)
{
    if (startString.empty()) {
        return false;
    }

    if (startString.size() > string.size()) {
        return false;
    }

    for (std::size_t i = 0; i < startString.size(); i++) {
        if (string[i] != startString[i]) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> split(const std::string& string, const std::string& delimeter)
{
    if (delimeter.empty()) {
        return {};
    }

    std::vector<std::string> result;

    auto begin = string.begin();
    std::size_t pos = 0;

    while ((pos = string.find(delimeter, pos)) != string.npos) {
        auto end = string.begin();
        std::advance(end, pos);
        std::string newString{begin, end};
        result.push_back(newString);
        pos += delimeter.size();
        begin = string.begin();
        std::advance(begin, pos);
    }

    std::string newString{begin, string.cend()};
    result.push_back(newString);

    return result;
}

std::string trim(const std::string& string)
{
    auto begin = string.begin();
    for (; begin != string.cend(); ++begin) {
        if (*begin != ' ') {
            break;
        }
    }

    if (begin == string.cend()) {
        return {};
    }

    auto end = string.end() - 1;
    for (; end != begin; --end) {
        if (*end != ' ') {
            break;
        }
    }

    return {begin, end + 1};
}
