#include "FileSolutionSource.h"
#include <fmt/format.h>
#include <fstream>
#include <iterator>

FileSolutionSource::FileSolutionSource(const std::string& filename)
{
    std::ifstream fileStream(filename, std::ios::binary);

    if (!fileStream.is_open()) {
        throw std::runtime_error{"Cannot open file"};
    }

    char c;
    while (fileStream.get(c)) {
        m_content.push_back(c);
    }
}

const std::vector<char>& FileSolutionSource::content() const
{
    return m_content;
}
