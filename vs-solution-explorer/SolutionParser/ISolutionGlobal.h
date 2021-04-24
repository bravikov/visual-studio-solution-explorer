#ifndef H_A3DD3998_8A6E_45A1_96F8_837E7845BF9C
#define H_A3DD3998_8A6E_45A1_96F8_837E7845BF9C

#include "ISolutionProject.h"
#include <string>
#include <unordered_map>
#include <list>

class ISolutionGlobal
{
public:
    struct NestedProject
    {
        ISolutionProject::Uuid projectUuid;
        ISolutionProject::Uuid parentProjectUuid;
    };

    struct SolutionConfiguration
    {
        std::string name;
        std::string platform;
    };

    struct ProjectConfiguration
    {
        std::string name;
        std::string platform;
    };

    struct ProjectConfigurationMap
    {
        ISolutionProject::Uuid projectUuid;
        SolutionConfiguration solutionConfiguration;
        ProjectConfiguration projectConfiguration;
    };

    using SolutionProperties = std::unordered_map<std::string, std::string>;
    using NestedProjects = std::list<NestedProject>;
    using SolutionConfigurations = std::list<SolutionConfiguration>;
    using ProjectConfigurations = std::list<ProjectConfigurationMap>;

    virtual ~ISolutionGlobal();
    virtual const std::string& solutionGuid() const = 0;
    virtual const SolutionProperties& solutionProperties() const = 0;
    virtual const NestedProjects& nestedProjects() const = 0;
    virtual const SolutionConfigurations& solutionConfigurations() const = 0;
    virtual const ProjectConfigurations& projectConfigurations() const = 0;
    virtual const ProjectConfigurations& buildedProjectConfigurations() const = 0;
};

#endif // H_A3DD3998_8A6E_45A1_96F8_837E7845BF9C
