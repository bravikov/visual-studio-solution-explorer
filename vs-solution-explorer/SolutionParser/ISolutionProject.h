#ifndef H_E02F6FD9_E281_456A_AC71_C80C568D67BE
#define H_E02F6FD9_E281_456A_AC71_C80C568D67BE

#include <string>
#include <list>

enum class SolutionProjectType;

class ISolutionProject
{
public:
    virtual ~ISolutionProject();
    virtual SolutionProjectType type() const = 0;
    virtual const std::string& name() const = 0;
    virtual const std::string& path() const = 0;
    virtual const std::string& uuid() const = 0;

    using Dependencies = std::list<std::string>;
    virtual const Dependencies& dependencies() const = 0;
};

#endif // H_E02F6FD9_E281_456A_AC71_C80C568D67BE
