#include "SolutionGlobal.h"

const std::string& SolutionGlobal::solutionGuid() const
{
    return m_solutionGuid;
}

const ISolutionGlobal::SolutionProperties& SolutionGlobal::solutionProperties() const
{
    return m_solutionProperties;
}

const ISolutionGlobal::NestedProjects& SolutionGlobal::nestedProjects() const
{
    return m_nestedProjects;
}

const ISolutionGlobal::SolutionConfigurations& SolutionGlobal::solutionConfigurations() const
{
    return m_solutionConfigurations;
}

const ISolutionGlobal::ProjectConfigurations& SolutionGlobal::projectConfigurations() const
{
    return m_projectConfigurations;
}

const ISolutionGlobal::ProjectConfigurations& SolutionGlobal::buildedProjectConfigurations() const
{
    return m_builedProjectConfigurations;
}
