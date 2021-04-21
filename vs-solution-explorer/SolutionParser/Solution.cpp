#include "Solution.h"

const std::string& Solution::formatVersion() const
{
    return m_formatVersion;
}

const std::string& Solution::visualStudioVersion() const
{
    return m_visualStudioVersion;
}

const std::string& Solution::minimumVisualStudioVersion() const
{
    return m_minimumVisualStudioVersion;
}

const ISolution::SolutionProjects& Solution::projects() const
{
    return m_solutionProjects;
}

const std::shared_ptr<ISolutionGlobal>& Solution::global() const
{
    return m_solutionGlobal;
}
