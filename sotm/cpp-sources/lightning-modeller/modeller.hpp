/*
 * modeller.hpp
 *
 *  Created on: 9 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef LIGHTNING_MODELLER_MODELLER_HPP_
#define LIGHTNING_MODELLER_MODELLER_HPP_

#include "seeds-generator.hpp"
#include "coulomb-selector.hpp"
#include "emission-writer.hpp"

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/electrostatics/electrostatics.hpp"
#include "sotm/time-iter/euler-explicit.hpp"
#include "sotm/time-iter/runge-kutta.hpp"
#include "sotm/math/random.hpp"
#include "sotm/output/graph-file-writer.hpp"
#include "sotm/math/functions.hpp"
#include "cic.hpp"

#include <boost/program_options.hpp>
#include <memory>

class Modeller
{
public:
    Modeller();
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
    void initEmissionWriter();
	void generateCondEvoParams();
    void initSeeds();
    void writeEmission();

	static std::string getTimeStr();

	sotm::ModelContext c;
	sotm::ElectrostaticPhysicalContext* m_physCont;
	std::unique_ptr<sotm::TimeIterator> m_timeIter;
	std::unique_ptr<sotm::FileWriteHook> m_fileWriteHook;
	std::unique_ptr<sotm::RungeKuttaIterator> m_rkIterator;
	std::unique_ptr<sotm::Field<1, 3>> m_externalPotential;
	std::unique_ptr<sotm::TrapezoidFunc> m_trapezoid;

	sotm::ContiniousIteratorParameters m_timeIterParams;

    std::unique_ptr<sotm::EmissionWriter> m_emission_writer;

    /// @todo: Each module should add its own group here by himself
	cic::Parameters m_p{
		"Lightning modeller command line options",
		cic::ParametersGroup(
		    "General",
		    "General options",
            cic::Parameter<bool>("no-gui", "Work without GUI", cic::ParamterType::cmdLine),
            cic::Parameter<bool>("benchmark", "Do not output data", cic::ParamterType::cmdLine),
            cic::Parameter<bool>("no-threads", "Run in signle thread", cic::ParamterType::cmdLine)
		),
		cic::ParametersGroup(
		    "Iter",
		    "Iterating options",
		    cic::Parameter<double>("step-min",       "Minimal integration step", 0.0),
		    cic::Parameter<double>("step-max",       "Maximal integration step", 1e-7),
		    cic::Parameter<double>("frame-duration", "File output frame duration", 1e-6),
		    cic::Parameter<double>("stop-time",      "Integration time limit", 1.0)
		),
		cic::ParametersGroup(
		    "Discharge",
		    "Gas discharge options",
		    cic::Parameter<double>("beta",                "Electrons conductivity relaxation decrement", 2e7),
		    cic::Parameter<double>("ioi-temp",            "TII critical temperature", 1500.0),
		    cic::Parameter<double>("cond-limit",          "Conductivity limit", 1e1),
            cic::Parameter<double>("field-cond-critical", "Critical field that maintain glow discharge. Beta calculated by this value. Minimal conductivity is 95% of it", 0.24e6),
            cic::Parameter<double>("field-connect-critical", "Critical average field for nodes connection", 0.3e6),
            cic::Parameter<double>("conductivity-initial", "Initial link conductivity", 1e-10)
		),
        cic::ParametersGroup(
            "Geometry",
            "Nodes and links geometry options",
            cic::Parameter<double>("node-radius-conductivity-default", "Node effective radius for new nodes from point of conductivity calculations. This is capacity exactly", 0.03),
            cic::Parameter<double>("node-radius-branching-default",    "Node effective radius for new nodes from point of branching. Surface E field will be calculated here", 0.05),
            cic::Parameter<double>("link-radius",                      "Link wire effective radius for conductivity and air heating", 0.001),
            cic::Parameter<double>("branch-step",                      "Link wire length when bush is branching", 0.3)
        ),
		cic::ParametersGroup(
		    "Seeds",
		    "Initial seeds options",
		    cic::Parameter<unsigned int>("seeds-number", "Count of seed streamers in zone", 1),
		    cic::Parameter<double>("seeds-zone-height",  "Height of zone where seeds will be generated", 15.0),
		    cic::Parameter<double>("seeds-zone-dia",     "Diameter of zone where seeda will be generated", 5),
            cic::Parameter<double>("seeds-min-dist",     "Minimal distance between seeds", 0.01),
            cic::Parameter<double>("seed-size",          "Size of seed's link", 0.4),
            cic::Parameter<double>("seed-radius-cond",   "Seeds' node radius for conductivity", 0.03),
            cic::Parameter<double>("seed-radius-branch", "Seeds' node radius for branching", 0.05),
            cic::Parameter<double>("seed-beta",          "Seeds' link beta", 2e7),
            cic::Parameter<double>("seed-field-cond-critical", "Seeds' link critical field", 0.3e6),
            cic::Parameter<bool>("seeds-z-uniform",      "Place seeds uniformly by z, without random"),
            cic::Parameter<bool>("seeds-dynamic",        "Add seeds in process of evolution"),
            cic::Parameter<size_t>("seeds-dynamic-count","Count of seeds that should be added dynamically in process of evolution", 10000),
            cic::Parameter<double>("seeds-dynamic-count-per-vol-per-sec", "Count of seeds appearing in 1m^3 per 1s", 100)
		),
		cic::ParametersGroup(
		    "Field",
		    "Electric field options",
		    cic::Parameter<double>("field",             "External field", 0.3e6),
		    cic::Parameter<double>("field-z-size",      "Field vertical size", 1000),
		    cic::Parameter<double>("field-z-recession", "Field recession zone size", 1000)
        ),
        cic::ParametersGroup(
            "Emission",
            "Emission calculation options",
            cic::Parameter<double>("receiver-x", "Electromagnetic wave receiver x", 500),
            cic::Parameter<double>("receiver-y", "Electromagnetic wave receiver y", 0),
            cic::Parameter<double>("receiver-z", "Electromagnetic wave receiver z", -500),
            cic::Parameter<double>("dt", "Field discretization on csv file", 1e-8),
            cic::Parameter<double>("save-period", "Real time period of dumps, seconds", 1)
        )
	};

    SeedsGenerator m_sg{m_p["Seeds"], c};
    CoulombSelector m_coulombSelector{m_p};

	int m_argc = 0;
	char** m_argv = nullptr;
};

#endif /* LIGHTNING_MODELLER_MODELLER_HPP_ */
