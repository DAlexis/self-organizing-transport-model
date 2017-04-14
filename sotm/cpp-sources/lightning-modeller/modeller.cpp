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
	namespace po = boost::program_options;
	po::options_description generalOptions("General options");
	generalOptions.add_options()
		("help,h", "Print help message")
		("no-gui,g", "Print help message")
		(m_stepMin.name().c_str(),       po::value<double>()->default_value(0.0), "Minimal integration step")
		(m_stepMax.name().c_str(),       po::value<double>()->default_value(1e-7), "Maximal integration step")
		(m_frameDuration.name().c_str(), po::value<double>()->default_value(1e-6), "File output frame duration")
		(m_stopTime.name().c_str(),      po::value<double>()->default_value(1), "integration time limit");

	po::options_description dischargeOptions("Gas discharge options");
	dischargeOptions.add_options()
		(m_beta.name().c_str(), po::value<double>()->default_value(2e7), "Conductivity relaxation coefficient")
		(m_ioiTemp.name().c_str(), po::value<double>()->default_value(1500), "IOI critical temperature")
		(m_condLimit.name().c_str(), po::value<double>()->default_value(1e1), "Conductivity limit")
		(m_conductivityCriticalField.name().c_str(), po::value<double>()->default_value(0.24e6), "Critical field that maintain glow discharge");

	po::options_description seedsOptions("Seeds options");
	seedsOptions.add_options()
		(m_seedsNumber.name().c_str(),     po::value<unsigned int>()->default_value(1), "Count of seed objects")
		(m_seedsZoneHeight.name().c_str(), po::value<double>()->default_value(15.0), "Height of zone where seed objects will be generated")
		(m_seedsZoneDia.name().c_str(),    po::value<double>()->default_value(5.0), "Diameter of zone where seed objects will be generated")
		(m_seedsZUniform.name().c_str(),   "Place seeds uniformly by z, without random")
		(m_seedsMinDist.name().c_str(),    po::value<double>()->default_value(1.0), "Minimal distance between seeds");

	po::options_description fieldOptions("Field options");
	fieldOptions.add_options()
		(m_field.name().c_str(),      po::value<double>()->default_value(0.3e6), "External field")
		(m_fieldScale.name().c_str(), po::value<double>()->default_value(1000), "Field vertical size")
		(m_fieldRecession.name().c_str(), po::value<double>()->default_value(1000), "Field recession size");

	po::options_description allOptions("Allowed options");
	allOptions
		.add(generalOptions)
		.add(dischargeOptions)
		.add(seedsOptions)
		.add(fieldOptions);


	try
	{
		po::store(po::parse_command_line(argc, argv, allOptions), m_cmdLineOptions);
		po::notify(m_cmdLineOptions);
		m_stepMin = m_cmdLineOptions[m_stepMin.name().c_str()].as<double>();
		m_stepMax = m_cmdLineOptions[m_stepMax.name().c_str()].as<double>();
		m_frameDuration = m_cmdLineOptions[m_frameDuration.name().c_str()].as<double>();
		m_stopTime = m_cmdLineOptions[m_stopTime.name().c_str()].as<double>();

		m_beta = m_cmdLineOptions[m_beta.name().c_str()].as<double>();
		m_conductivityCriticalField = m_cmdLineOptions[m_conductivityCriticalField.name().c_str()].as<double>();
		m_field = m_cmdLineOptions[m_field.name().c_str()].as<double>();
		m_ioiTemp = m_cmdLineOptions[m_ioiTemp.name().c_str()].as<double>();
		m_condLimit = m_cmdLineOptions[m_condLimit.name().c_str()].as<double>();

		m_fieldScale = m_cmdLineOptions[m_fieldScale.name().c_str()].as<double>();
		m_fieldRecession = m_cmdLineOptions[m_fieldRecession.name().c_str()].as<double>();

		m_seedsNumber = m_cmdLineOptions[m_seedsNumber.name().c_str()].as<unsigned int>();
		m_seedsZoneHeight = m_cmdLineOptions[m_seedsZoneHeight.name().c_str()].as<double>();
		m_seedsZoneDia = m_cmdLineOptions[m_seedsZoneDia.name().c_str()].as<double>();
		m_seedsMinDist = m_cmdLineOptions[m_seedsMinDist.name().c_str()].as<double>();
		m_seedsZUniform = m_cmdLineOptions.count(m_seedsZUniform.name());
	}
	catch (po::error& e)
	{
		cerr << "Command line parsing error: " << e.what() << endl;
		return false;
	}

	if (m_cmdLineOptions.count("help"))
	{
		cout << allOptions << endl;
		return false;
	}

	return true;
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

	//iter.run();

	if (!m_cmdLineOptions.count("no-gui"))
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
	m_fileWriteHook.reset(new FileWriteHook(&c, nullptr, m_frameDuration));
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
	outputFile << m_parameters;
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
	m_trapezoid.reset(new TrapezoidFunc(-m_field, m_fieldScale, m_fieldRecession));
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

	m_physCont->conductivityLimit = m_condLimit;
	m_physCont->ionizationOverheatingInstFunc = SmoothedLocalStepFunction(m_ioiTemp, 50);

	Vector<3> externalField{0.0, 0.0, m_field};
	m_physCont->externalPotential = m_externalPotential.get();
	m_physCont->externalFieldValue = m_field;

	generateCondEvoParams();

	m_timeIter->setTime(0.0);
	m_timeIter->setStep(m_stepMax / 100);
	m_timeIter->setStepBounds(m_stepMin, m_stepMax);
	m_timeIter->continiousIterParameters().autoStepAdjustment = true;
	m_timeIter->setStopTime(m_stopTime);
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
	m_physCont->linkBeta = m_beta.get();
	m_physCont->linkEta = m_physCont->linkBeta / sqr(m_conductivityCriticalField.get());
}

void Modeller::genSeeds()
{
	if (m_seedsNumber == 1)
	{
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Vector<3>(0, 0, -0.2));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Vector<3>(0, 0, +0.2));

		PtrWrap<Link> l = PtrWrap<Link>::make(&c);

		l->connect(n1, n2);
	} else {
		for (unsigned int i=0; i<m_seedsNumber; i++)
		{
			double d=0.0;
			Vector<3> p;
			do
			{
				p[0] = Random::uniform(-m_seedsZoneDia, m_seedsZoneDia);
				p[1] = Random::uniform(-m_seedsZoneDia, m_seedsZoneDia);
				if (m_seedsZUniform)
					p[2] = -m_seedsZoneHeight + 2*m_seedsZoneHeight / (m_seedsNumber-1) * i;
				else
					p[2] = Random::uniform(-m_seedsZoneHeight, m_seedsZoneHeight);

				auto nearest = c.graphRegister.getNearestNode(p);
				if (nearest == nullptr)
					break;
				d = (nearest->pos-p).len();
			} while (d < m_seedsMinDist);

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
