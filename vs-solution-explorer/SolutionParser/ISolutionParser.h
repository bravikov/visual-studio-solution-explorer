#ifndef H_89F18022_A880_4133_B697_A74D314216A1
#define H_89F18022_A880_4133_B697_A74D314216A1

#include "ISolutionSource.h"
#include "ISolution.h"
#include <memory>

class ISolutionParser
{
public:
    enum class ErrorCode {Ok, Utf8BomNotFound, BadParserState, ParsingError};

    struct Error
    {
        ErrorCode code;
        std::string description;
    };

    virtual ~ISolutionParser();
    virtual std::unique_ptr<ISolution> parse(const ISolutionSource& solutionSource) = 0;

    virtual const Error& lastError() const = 0;
};

#endif // H_89F18022_A880_4133_B697_A74D314216A1
