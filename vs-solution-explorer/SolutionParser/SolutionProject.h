#ifndef H_D074368E_89F5_44B0_87D0_F3E36BB045C3
#define H_D074368E_89F5_44B0_87D0_F3E36BB045C3

#include "ISolutionProject.h"

class SolutionProject: public ISolutionProject
{
public:
    SolutionProject();

    SolutionProjectType type() const override;
    const std::string& name() const override;
    const std::string& path() const override;
    const Uuid& uuid() const override;
    const Dependencies& dependencies() const override;

    SolutionProjectType m_type;
    std::string m_name;
    std::string m_path;
    Uuid m_uuid;
    Dependencies m_dependencies;
};

#endif // H_D074368E_89F5_44B0_87D0_F3E36BB045C3
