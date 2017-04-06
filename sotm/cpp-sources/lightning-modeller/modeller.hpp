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
#include "sotm/math/functions.hpp"

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
	void initExternalPotential();
	void initScalersAndColors();
	void initParameters();
	void initTimeIterator();
	void generateCondEvoParams();
	void genSeeds();

	static std::string getTimeStr();

	sotm::ModelContext c;
	sotm::ElectrostaticPhysicalContext* m_physCont;
	std::unique_ptr<sotm::TimeIterator> m_timeIter;
	std::unique_ptr<sotm::FileWriteHook> m_fileWriteHook;
	std::unique_ptr<sotm::RungeKuttaIterator> m_rkIterator;
	std::unique_ptr<sotm::Field<1, 3>> m_externalPotential;
	std::unique_ptr<sotm::TrapezoidFunc> m_trapezoid;

	sotm::ContiniousIteratorParameters m_timeIterParams;

	sotm::ParametersGroup m_parameters{"LightningModellerCommandLine"};
	sotm::Parameter<double> m_frameDuration{m_parameters, "frameDuration"};
	sotm::Parameter<double> m_stepMin{m_parameters, "stepMin"};
	sotm::Parameter<double> m_stepMax{m_parameters, "stepMax"};
	sotm::Parameter<double> m_stopTime{m_parameters, "stopTime"};


	sotm::Parameter<double> m_beta{m_parameters, "beta"};
	sotm::Parameter<double> m_ioiTemp{m_parameters, "ioiTemp"};
	sotm::Parameter<double> m_condLimit{m_parameters, "condLimit"};

	sotm::Parameter<unsigned int> m_seedsNumber{m_parameters, "seedsNumber"};
	sotm::Parameter<double> m_seedsZoneHeight{m_parameters, "seedsZoneHeight"};
	sotm::Parameter<double> m_seedsZoneDia{m_parameters, "seedsZoneDia"};
	sotm::Parameter<double> m_seedsMinDist{m_parameters, "seedsMinDist"};

	sotm::Parameter<double> m_conductivityCriticalField{m_parameters, "conductivityCriticalField"};
	sotm::Parameter<double> m_field{m_parameters, "field"};
	sotm::Parameter<double> m_fieldScale{m_parameters, "field_scale"};
	sotm::Parameter<double> m_fieldRecession{m_parameters, "field_recession"};

	boost::program_options::variables_map m_cmdLineOptions;
	int m_argc;
	char** m_argv;
};

#endif /* LIGHTNING_MODELLER_MODELLER_HPP_ */
