#ifndef LINKSFINDER_HPP
#define LINKSFINDER_HPP

#include <string>

class LinksFinder
{
public:
    LinksFinder(const std::string& filename);

    bool getMinMaxZ(double tempThreshold, double& min, double& max);

private:
    const std::string m_filname;
};

#endif // LINKSFINDER_HPP
