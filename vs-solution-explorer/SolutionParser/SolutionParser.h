#ifndef H_2C66BFE0_A114_42A9_822F_891DEC0D2B2A
#define H_2C66BFE0_A114_42A9_822F_891DEC0D2B2A

#include "ISolutionParser.h"
#include <memory>

class SolutionParser: public ISolutionParser
{
public:
    std::unique_ptr<ISolution> parse(const ISolutionSource& solutionSource) override;
    const Error& lastError() const override;

private:
    Error m_lastError{ErrorCode::Ok, ""};
};

#endif // H_2C66BFE0_A114_42A9_822F_891DEC0D2B2A
