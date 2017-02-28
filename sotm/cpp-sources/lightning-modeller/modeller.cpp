#include "modeller.hpp"

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
		("no-gui,g", "Print help message");

	po::options_description allOptions("Allowed options");
	allOptions
		.add(generalOptions);


	try
	{
		po::store(po::parse_command_line(argc, argv, allOptions), m_cmdLineOptions);
		po::notify(m_cmdLineOptions);
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

	//EulerExplicitIterator continiousIterator;
	RungeKuttaIterator continiousIterator;
	//TimeIterator m_timeIter(&c, &continiousIterator, &c);
	m_timeIter.reset(new TimeIterator(&c, &continiousIterator, &c));

	initParameters();
	initParameters1();

	FileWriteHook fwh(&c, nullptr, m_timeIter->getTimestepMax()*10);
	fwh.setFilenamePrefix(std::string("lightmod_") + getTimeStr());
	m_timeIter->addHook(&fwh);

	m_physCont->chargeScaler.fixValue(0.0, 0.5);
	m_physCont->chargeColorMapper.setBlueRed();

	m_physCont->conductivityScaler.setScale(Scaler::Scale::log);
	m_physCont->conductivityColorMapper.setGreenYellow();

	{ // Scope to remove pointers
/*
		for (int i=0; i<10; i++)
		{
			double x = Random::uniform(-2, 2);
			double y = Random::uniform(-2, 2);
			double z = Random::uniform(-3, 3);

			/// Building initial tree
			PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Vector<3>(x, y, z-0.2));
			PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Vector<3>(x, y, z+0.2));

			PtrWrap<Link> l = PtrWrap<Link>::make(&c);

			l->connect(n1, n2);
		}*/
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Vector<3>(0, 0, -0.2));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Vector<3>(0, 0, +0.2));

		PtrWrap<Link> l = PtrWrap<Link>::make(&c);

		l->connect(n1, n2);
		//static_cast<ElectrostaticNodePayload*>(n1->payload.get())->setCharge(4e-6);
		//static_cast<ElectrostaticNodePayload*>(n2->payload.get())->setCharge(4e-6);
	}

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

void Modeller::initFileOutput()
{
	m_fileWriteHook.reset(new FileWriteHook(&c, nullptr, m_timeIter->getTimestepMax()*10));
	m_fileWriteHook->setFilenamePrefix(std::string("lightmod_") + getTimeStr());
	m_timeIter->addHook(m_fileWriteHook.get());
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

	m_physCont->nodeRadiusConductivity = 0.05;
	m_physCont->nodeRadiusBranching = 0.05;
	m_physCont->linkRadius = 0.001;
	m_physCont->branchingStep = 0.3;

	m_physCont->connectionCriticalField = 0.3e6;
	m_physCont->connectionMaximalDist = m_physCont->branchingStep.get();

	m_physCont->initialConductivity = 1e-5;
	m_physCont->minimalConductivity = m_physCont->initialConductivity * 0.95;

	m_physCont->conductivityLimit = 1e2;
	m_physCont->ionizationOverheatingInstFunc = SmoothedLocalStepFunction(1000, 50);

	m_physCont->linkEta = 1e-5; // 1e-4; // 1e-5;
	m_physCont->linkBeta = 1e4;
	Vector<3> externalField{0.0, 0.0, 0.2e6};
	m_physCont->externalConstField = externalField;


	m_timeIter->setTime(0.0);
	m_timeIter->setStep(1e-8);
    //timeIter->setStepBounds(0.0000000002, 0.0000002);
	m_timeIter->setStepBounds(0.0, 1e-7);
	m_timeIter->continiousIterParameters().autoStepAdjustment = true;
	m_timeIter->setStopTime(1e-1);
	//m_timeIter->setStopTime(7e-4);
}

void Modeller::initParameters1()
{
	Vector<3> externalField{0.0, 0.0, 0.3e6};
	m_physCont->externalConstField = externalField;
	m_physCont->nodeRadiusConductivity = 0.03;

	double betaDominationFactor = 1.5;
	m_physCont->linkBeta = 2e7;
	m_physCont->linkEta = m_physCont->linkBeta / sqr(m_physCont->externalConstField[2]) * betaDominationFactor;

	m_physCont->initialConductivity = 1e-10;
	m_physCont->minimalConductivity = m_physCont->initialConductivity * 0.95;
}

std::string Modeller::getTimeStr()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
	return oss.str();
}
