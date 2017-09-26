#include "modeller.hpp"
#include "config.hpp"

#include "sotm/output/graph-file-writer.hpp"
#include "sotm-gui/gui.hpp"
#include "sotm/math/random.hpp"
#include "sotm/utils/const.hpp"

#include <iostream>
#include <sstream>
#include <cmath>

using namespace sotm;
using namespace std;


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

	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new ElectrostaticNodePayloadFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new ElectrostaticLinkPayloadFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new ElectrostaticPhysicalContext()));

	c.parallelSettings.parallelContiniousIteration.calculateSecondaryValues = true;
	c.parallelSettings.parallelBifurcationIteration.prepareBifurcation = true;

	m_physCont = static_cast<ElectrostaticPhysicalContext*>(c.physicalContext());

	initTimeIterator();
	initExternalPotential();
	initParameters();
	initScalersAndColors();

	std::string filenamePrefix = std::string("lightmod_") + getTimeStr();
	initFileOutput(filenamePrefix);
	createParametersFile(filenamePrefix);
	createProgramCofigurationFile(filenamePrefix);

	genSeeds();

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
	ElectrostaticPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);
	cout << "Exiting" << endl;
}

void Modeller::initFileOutput(const std::string& prefix)
{
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
	outputFile << m_physCont->parameters;
	outputFile << std::endl;
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
				if (E > 0.5e6) // 20 kV/cm
					return (E - 0.5e6)/2e6 * 5e7;
				if (E < -1e6) // 20 kV/cm
					return (-E - 1e6)/2e6 * 5e7;
				return 0.0;
			}
	);

	m_physCont->nodeRadiusConductivity = 0.03;
	m_physCont->nodeRadiusBranching = 0.05;
	m_physCont->linkRadius = 0.001;
	m_physCont->branchingStep = 0.3;

	m_physCont->connectionCriticalField = 0.3e6;
	m_physCont->connectionMaximalDist = m_physCont->branchingStep.get();

	m_physCont->initialConductivity = 1e-10;
	m_physCont->minimalConductivity = m_physCont->initialConductivity * 0.95;

	m_physCont->conductivityLimit = m_p["Discharge"].get<double>("cond-limit");
	m_physCont->ionizationOverheatingInstFunc = SmoothedLocalStepFunction(m_p["Discharge"].get<double>("ioi-temp"), 50);

	Vector<3> externalField{0.0, 0.0, m_p["Field"].get<double>("field")};
	m_physCont->externalPotential = m_externalPotential.get();
	m_physCont->externalFieldValue = m_p["Field"].get<double>("field");

	generateCondEvoParams();

	m_timeIter->setTime(0.0);
	m_timeIter->setStep(m_p["Iter"].get<double>("step-max") / 100);
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
	m_physCont->linkBeta = m_p["Discharge"].get<double>("beta");
	m_physCont->linkEta = m_physCont->linkBeta / sqr(m_p["Discharge"].get<double>("field-cond-critical"));
}

void Modeller::genSeeds()
{
	unsigned int seedsCount = m_p["Seeds"].get<unsigned int>("seeds-number");
	double dia = m_p["Seeds"].get<double>("seeds-zone-dia");
	double height = m_p["Seeds"].get<double>("seeds-zone-height");
	double minDist = m_p["Seeds"].get<double>("seeds-min-dist");
	bool uniform = m_p["Seeds"].get<bool>("seeds-z-uniform");

	if (seedsCount == 1)
	{
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Vector<3>(0, 0, -0.2));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Vector<3>(0, 0, +0.2));

		PtrWrap<Link> l = PtrWrap<Link>::make(&c);

		l->connect(n1, n2);
	} else {
		for (unsigned int i=0; i<seedsCount; i++)
		{
			double d=0.0;
			Vector<3> p;
			do
			{
				p[0] = Random::uniform(-dia, dia);
				p[1] = Random::uniform(-dia, dia);
				if (uniform)
					p[2] = -height + 2*height / (seedsCount-1) * i;
				else
					p[2] = Random::uniform(-height, height);

				auto nearest = c.graphRegister.getNearestNode(p);
				if (nearest == nullptr)
					break;
				d = (nearest->pos-p).len();
			} while (d < minDist);

			/// Building initial tree
			PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Vector<3>(p[0], p[1], p[2]-0.2));
			PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Vector<3>(p[0], p[1], p[2]+0.2));

			PtrWrap<Link> l = PtrWrap<Link>::make(&c);

			l->connect(n1, n2);
		}
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
