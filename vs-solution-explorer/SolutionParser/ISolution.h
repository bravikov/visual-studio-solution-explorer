#ifndef H_181A71D1_DE1F_4DD2_9370_8E9084A94CA2
#define H_181A71D1_DE1F_4DD2_9370_8E9084A94CA2

#include <string>
#include <memory>
#include <list>

class ISolutionProject;
class ISolutionGlobal;

class ISolution
{
public:
    using SolutionProjects = std::list<std::shared_ptr<ISolutionProject>>;

    virtual ~ISolution();
    virtual const std::string& formatVersion() const = 0;
    virtual const std::string& visualStudioVersion() const = 0;
    virtual const std::string& minimumVisualStudioVersion() const = 0;
    virtual const SolutionProjects& projects() const = 0;
    virtual const std::shared_ptr<ISolutionGlobal>& global() const = 0;
};

#endif // H_181A71D1_DE1F_4DD2_9370_8E9084A94CA2
