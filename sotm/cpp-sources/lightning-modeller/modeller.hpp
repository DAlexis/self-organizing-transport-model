/*
 * modeller.hpp
 *
 *  Created on: 9 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef LIGHTNING_MODELLER_MODELLER_HPP_
#define LIGHTNING_MODELLER_MODELLER_HPP_

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/electrostatics/electrostatics-simple.hpp"
#include "sotm/time-iter/euler-explicit.hpp"
#include "sotm/time-iter/runge-kutta.hpp"
#include "sotm/math/random.hpp"

#include <boost/program_options.hpp>

class Modeller
{
public:
	bool parseCmdLineArgs(int argc, char** argv);
	void run();

private:
	void initParameters(sotm::ElectrostaticPhysicalContext* physCont, sotm::TimeIterator* timeIter);
	void initParameters1(sotm::ElectrostaticPhysicalContext* physCont, sotm::TimeIterator* timeIter);

	static std::string getTimeStr();

	boost::program_options::variables_map m_cmdLineOptions;
	int m_argc;
	char** m_argv;
};

#endif /* LIGHTNING_MODELLER_MODELLER_HPP_ */
