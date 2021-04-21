#ifndef H_B358A7AE_7DAD_40D0_861E_9F5923DDB452
#define H_B358A7AE_7DAD_40D0_861E_9F5923DDB452

#include "ISolutionGlobal.h"

class SolutionGlobal: public ISolutionGlobal
{
public:
    const std::string& solutionGuid() const override;
    const SolutionProperties& solutionProperties() const override;
    const NestedProjects& nestedProjects() const override;
    const SolutionConfigurations& solutionConfigurations() const override;
    const ProjectConfigurations& projectConfigurations() const override;
    const ProjectConfigurations& buildedProjectConfigurations() const override;

    std::string m_solutionGuid;
    SolutionProperties m_solutionProperties;
    NestedProjects m_nestedProjects;
    SolutionConfigurations m_solutionConfigurations;
    ProjectConfigurations m_projectConfigurations;
    ProjectConfigurations m_builedProjectConfigurations;
};
#endif // H_B358A7AE_7DAD_40D0_861E_9F5923DDB452
