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
#include "sotm/output/graph-file-writer.hpp"

#include <boost/program_options.hpp>
#include <memory>

class Modeller
{
public:
	bool parseCmdLineArgs(int argc, char** argv);
	void run();

private:
	void initFileOutput(const std::string& prefix);
	void initScalersAndColors();
	void initParameters();
	void generateCondEvoParams();

	static std::string getTimeStr();

	sotm::ModelContext c;
	sotm::ElectrostaticPhysicalContext* m_physCont;
	std::unique_ptr<sotm::TimeIterator> m_timeIter;
	std::unique_ptr<sotm::FileWriteHook> m_fileWriteHook;

	double m_beta = 0.0;
	double m_conductivityCriticalField = 0.0;
	double m_field = 0.0;
	double m_ioiTemp = 0.0;

	boost::program_options::variables_map m_cmdLineOptions;
	int m_argc;
	char** m_argv;
};

#endif /* LIGHTNING_MODELLER_MODELLER_HPP_ */
