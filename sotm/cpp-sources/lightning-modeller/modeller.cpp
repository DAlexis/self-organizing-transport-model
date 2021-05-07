#include "modeller.hpp"
#include "config.hpp"

#include "sotm/output/graph-file-writer.hpp"
#include "sotm-gui/gui.hpp"
#include "sotm/math/random.hpp"
#include "sotm/utils/const.hpp"

#include <iostream>
#include <sstream>
#include <cmath>
#include <fenv.h>

using namespace sotm;
using namespace std;

Modeller::Modeller()
{
#ifdef DEBUG
    feenableexcept(FE_INVALID | FE_OVERFLOW);
#endif
}

bool Modeller::parseCmdLineArgs(int argc, char** argv)
{
	m_argc = argc;
	m_argv = argv;
	cic::PreconfiguredOperations::addGeneralOptions(m_p);
	bool needRun = false;
	try {
		needRun = cic::PreconfiguredOperations::quickReadConfiguration(
		    m_p,
		    {
		        "/etc/sotm.conf",
		        "~/.config/sotm/sotm.conf",
		        "sotm.conf"
		    },
		    argc,
		    argv
		);
	} catch (std::exception &ex) {
		cerr << "Error while reading configuration: " << ex.what() << endl;
		return false;
	}
	return needRun;
}

void Modeller::run()
{
	Random::randomize(0);

    c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new ElectrostaticPhysicalContext()));
    m_physCont = static_cast<ElectrostaticPhysicalContext*>(c.physicalContext());

    c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new ElectrostaticNodePayloadFactory(*m_physCont)));
    c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new ElectrostaticLinkPayloadFactory(*m_physCont)));

    m_coulombSelector.addCoulombCalculator(*m_physCont);

    if (!m_p["General"].get<bool>("no-threads"))
    {
        c.parallelSettings.parallelContiniousIteration.calculateSecondaryValues = true;
        c.parallelSettings.parallelContiniousIteration.calculateRHS = true;
        c.parallelSettings.parallelContiniousIteration.addRHSToDelta = true;
        c.parallelSettings.parallelContiniousIteration.makeSubIteration = true;
        c.parallelSettings.parallelContiniousIteration.step = true;
        c.parallelSettings.parallelBifurcationIteration.prepareBifurcation = true;
    }

	initTimeIterator();
	initExternalPotential();
	initParameters();
	initScalersAndColors();

	std::string filenamePrefix = std::string("lightmod_") + getTimeStr();
	initFileOutput(filenamePrefix);
	createParametersFile(filenamePrefix);
	createProgramCofigurationFile(filenamePrefix);

    initSeeds();

	c.initAllPhysicalPayloads();

	// Time iteration

	if (! m_p["General"].get<bool>("no-gui") )
	{
		// Running GUI
		GUI gui(&c, m_timeIter.get());
		gui.renderPreferences()->lineWidth = false;
		gui.setFrameOptions(0.0000001, 30);
		gui.run(m_argc, m_argv);
	} else {
		m_timeIter->run();
	}

	cout << "Destroying graph" << endl;
    c.destroyAll();
	cout << "Exiting" << endl;
}

void Modeller::initFileOutput(const std::string& prefix)
{
    if (m_p["General"].get<bool>("benchmark"))
    {
        return;
    }
	m_fileWriteHook.reset(new FileWriteHook(&c, nullptr, m_p["Iter"].get<double>("frame-duration")));
	m_fileWriteHook->setFilenamePrefix(prefix);
	m_timeIter->addHook(m_fileWriteHook.get());
}

void Modeller::createParametersFile(const std::string& prefix)
{
	std::string filename = prefix + "_parameters.txt";
	ofstream outputFile(filename.c_str(), std::ios::out);
	if (!outputFile.is_open())
	{
		cerr << "ERROR: Cannot open file " << filename << " to write current parameters!" << endl;
		return;
	}
    m_p.writeIni(outputFile);
}

void Modeller::createProgramCofigurationFile(const std::string& prefix)
{
	string name = prefix+"_config.txt";
	ofstream outputFile(name.c_str(), ios::out);
	if (!outputFile.is_open())
	{
		cerr << "ERROR: Cannot save config file " << name << endl;
	}
	outputFile << "git_commit = " << Config::Build::commitTag << endl;
	outputFile << "build = " << Config::Build::build << endl;
	outputFile << "build_time = " << Config::Build::buildTime << endl;
}

void Modeller::initExternalPotential()
{
	m_trapezoid.reset(
	    new TrapezoidFunc(
	        - m_p["Field"].get<double>("field"),
	        m_p["Field"].get<double>("field-z-size"),
	        m_p["Field"].get<double>("field-z-recession")
	    )
	);
	m_externalPotential.reset(
		new FieldScalar1D<3>(
			[this](double z) { return (*m_trapezoid)(z); },
			{0.0, 0.0, 1.0}
		)
	);
}

void Modeller::initScalersAndColors()
{
	m_physCont->chargeScaler.fixValue(0.0, 0.5);
	m_physCont->chargeColorMapper.setBlueRed();

	m_physCont->conductivityScaler.setScale(Scaler::Scale::log);
	m_physCont->conductivityColorMapper.setGreenYellow();
}

void Modeller::initParameters()
{
	m_physCont->setDischargeFunc(
		[](double E) -> double
		{
			if (E > 0.5e6)
				return (E - 0.5e6)/2e6 * 5e7;
			if (E < -1e6)
				return (-E - 1e6)/2e6 * 5e7;
			return 0.0;
		}
	);

    m_physCont->nodeRadiusConductivityDefault = m_p["Geometry"].get<double>("node-radius-conductivity-default");
    m_physCont->nodeRadiusBranchingDefault = m_p["Geometry"].get<double>("node-radius-branching-default");
    m_physCont->linkRadius = m_p["Geometry"].get<double>("link-radius");
    m_physCont->branchingStep = m_p["Geometry"].get<double>("branch-step");

    m_physCont->connectionCriticalField = m_p["Discharge"].get<double>("field-connect-critical");
	m_physCont->connectionMaximalDist = m_physCont->branchingStep.get();

    m_physCont->initialConductivity = m_p["Discharge"].get<double>("conductivity-initial");
	m_physCont->minimalConductivity = m_physCont->initialConductivity * 0.95;

	m_physCont->conductivityLimit = m_p["Discharge"].get<double>("cond-limit");
	m_physCont->ionizationOverheatingInstFunc = SmoothedLocalStepFunction(m_p["Discharge"].get<double>("ioi-temp"), 50);

	StaticVector<3> externalField{0.0, 0.0, m_p["Field"].get<double>("field")};
	m_physCont->externalPotential = m_externalPotential.get();

	generateCondEvoParams();

	m_timeIter->setTime(0.0);
    m_timeIter->setStep(m_p["Iter"].get<double>("step-max"));
	m_timeIter->setStepBounds(m_p["Iter"].get<double>("step-min"), m_p["Iter"].get<double>("step-max"));
	m_timeIter->continiousIterParameters().autoStepAdjustment = true;
	m_timeIter->setStopTime(m_p["Iter"].get<double>("stop-time"));
}

void Modeller::initTimeIterator()
{
	m_rkIterator.reset(new RungeKuttaIterator());
	m_rkIterator->setParameters(&m_timeIterParams);
	m_timeIter.reset(new TimeIterator(&c, m_rkIterator.get(), &c));
	m_timeIter->continiousIterParameters().outputVerboseLevel = ContiniousIteratorParameters::VerboseLevel::more;
}

void Modeller::generateCondEvoParams()
{
    m_physCont->linkBetaDefault = m_p["Discharge"].get<double>("beta");
    m_physCont->linkEtaDefault = ElectrostaticNodePayload::etaFromCriticalField(m_p["Discharge"].get<double>("field-cond-critical"), m_physCont->linkBetaDefault);
}

void Modeller::initSeeds()
{
    m_sg.parseConfig();
    m_sg.generateInitial();
    if (m_p["Seeds"].get<bool>("seeds-dynamic"))
    {
        m_timeIter->addHook(&m_sg.hook());
    }
}

std::string Modeller::getTimeStr()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
	return oss.str();
}
