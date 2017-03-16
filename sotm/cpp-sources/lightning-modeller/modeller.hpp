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
	void createParametersFile(const std::string& prefix);
	void createProgramCofigurationFile(const std::string& prefix);
	void initScalersAndColors();
	void initParameters();
	void generateCondEvoParams();
	void genSeeds();

	static std::string getTimeStr();

	sotm::ModelContext c;
	sotm::ElectrostaticPhysicalContext* m_physCont;
	std::unique_ptr<sotm::TimeIterator> m_timeIter;
	std::unique_ptr<sotm::FileWriteHook> m_fileWriteHook;

	sotm::ParametersGroup m_parameters{"LightningModellerCommandLine"};
	sotm::Parameter<double> m_beta{m_parameters, "beta"};
	sotm::Parameter<double> m_conductivityCriticalField{m_parameters, "conductivityCriticalField"};
	sotm::Parameter<double> m_field{m_parameters, "field"};
	sotm::Parameter<double> m_ioiTemp{m_parameters, "ioiTemp"};
	sotm::Parameter<double> m_condLimit{m_parameters, "condLimit"};

	sotm::Parameter<unsigned int> m_seedsNumber{m_parameters, "seedsNumber"};
	sotm::Parameter<double> m_seedsZoneHeight{m_parameters, "seedsZoneHeight"};
	sotm::Parameter<double> m_seedsZoneDia{m_parameters, "seedsZoneDia"};
	sotm::Parameter<double> m_seedsMinDist{m_parameters, "seedsMinDist"};

	boost::program_options::variables_map m_cmdLineOptions;
	int m_argc;
	char** m_argv;
};

#endif /* LIGHTNING_MODELLER_MODELLER_HPP_ */
