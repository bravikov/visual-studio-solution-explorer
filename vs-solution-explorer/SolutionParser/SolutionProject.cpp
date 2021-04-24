#include "SolutionProject.h"
#include "SolutionProjectType.h"

SolutionProject::SolutionProject()
    : m_type{SolutionProjectType::Unknown}
{
}

SolutionProjectType SolutionProject::type() const
{
    return m_type;
}

const std::string& SolutionProject::name() const
{
    return m_name;
}

const std::string& SolutionProject::path() const
{
    return m_path;
}

const ISolutionProject::Uuid& SolutionProject::uuid() const
{
    return m_uuid;
}

const ISolutionProject::Dependencies& SolutionProject::dependencies() const
{
    return m_dependencies;
}
