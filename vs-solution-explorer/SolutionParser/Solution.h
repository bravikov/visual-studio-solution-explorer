#ifndef H_6D590F71_84F0_41ED_885C_AA2F32647D11
#define H_6D590F71_84F0_41ED_885C_AA2F32647D11

#include "ISolution.h"

class Solution: public ISolution
{
public:
    const std::string& formatVersion() const override;
    const std::string& visualStudioVersion() const override;
    const std::string& minimumVisualStudioVersion() const override;
    const SolutionProjects& projects() const override;
    const std::shared_ptr<ISolutionGlobal>& global() const override;

    std::string m_formatVersion;
    std::string m_visualStudioVersion;
    std::string m_minimumVisualStudioVersion;
    SolutionProjects m_solutionProjects;
    std::shared_ptr<ISolutionGlobal> m_solutionGlobal;
};

#endif // H_6D590F71_84F0_41ED_885C_AA2F32647D11
