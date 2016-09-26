#include "sotm/math/random.hpp"

using namespace sotm;

boost::mt19937 Random::randomGenerator;

void Random::randomize(unsigned int parameter)
{
    randomGenerator.seed(parameter);
}

void Random::randomizeUsingTime()
{
    randomize(static_cast<unsigned int>(time(0)));
}

double Random::gaussian(double center, double dispersion)
{
    if (dispersion < 1e-15)
        return center;
    boost::normal_distribution<> distr(center, dispersion);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > varGen(randomGenerator, distr);
    return varGen();
}

double Random::uniform(double from, double to)
{
    boost::uniform_real<> distr(from, to);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<> > varGen(randomGenerator, distr);
    return varGen();
}

