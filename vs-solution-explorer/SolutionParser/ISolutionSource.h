#ifndef H_954BAD09_5168_4577_B462_CF17092999E8
#define H_954BAD09_5168_4577_B462_CF17092999E8

#include <vector>

class ISolutionSource
{
public:
    virtual ~ISolutionSource();
    virtual const std::vector<char>& content() const = 0;
};

#endif // H_954BAD09_5168_4577_B462_CF17092999E8
