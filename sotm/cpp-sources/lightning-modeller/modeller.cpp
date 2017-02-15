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
	ModelContext c;

	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new ElectrostaticNodePayloadFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new ElectrostaticLinkPayloadFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new ElectrostaticPhysicalContext()));

	c.parallelSettings.parallelContiniousIteration.calculateSecondaryValues = true;
	c.parallelSettings.parallelBifurcationIteration.prepareBifurcation = true;

	ElectrostaticPhysicalContext* physCont = static_cast<ElectrostaticPhysicalContext*>(c.physicalContext());

	//EulerExplicitIterator continiousIterator;
	RungeKuttaIterator continiousIterator;
	TimeIterator iter(&c, &continiousIterator, &c);
	initParameters(physCont, &iter);
	initParameters1(physCont, &iter);

	FileWriteHook fwh(&c, nullptr, iter.getTimestepMax()*10);
	fwh.setFilenamePrefix(std::string("lightmod_") + getTimeStr());
	iter.addHook(&fwh);

	physCont->chargeScaler.fixValue(0.0, 0.5);
	physCont->chargeColorMapper.setBlueRed();

	physCont->conductivityScaler.setScale(Scaler::Scale::log);
	physCont->conductivityColorMapper.setGreenYellow();

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
		GUI gui(&c, &iter);
		gui.renderPreferences()->lineWidth = false;
		gui.setFrameOptions(0.000001, 30);
		gui.run(m_argc, m_argv);
	} else {
		iter.run();
	}

	cout << "Destroying graph" << endl;
	ElectrostaticPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);
	cout << "Exiting" << endl;
}


void Modeller::initParameters(ElectrostaticPhysicalContext* physCont, TimeIterator* timeIter)
{
	physCont->setDischargeFunc(
			[](double E) -> double
			{
				if (E > 0.5e6) // 20 kV/cm
					return (E - 0.5e6)/2e6 * 5e7;
				if (E < -1e6) // 20 kV/cm
					return (-E - 1e6)/2e6 * 5e7;
				return 0.0;
			}
	);

	physCont->nodeRadiusConductivity = 0.05;
	physCont->nodeRadiusBranching = 0.05;
	physCont->linkRadius = 0.001;
	physCont->branchingStep = 0.3;

	physCont->connectionCriticalField = 0.3e6;
	physCont->connectionMaximalDist = physCont->branchingStep.get();

	physCont->initialConductivity = 1e-5;
	physCont->minimalConductivity = physCont->initialConductivity * 0.95;

	physCont->conductivityLimit = 1e0;
	/*
	physCont->ionizationOverheatingInstFunc = [](double temp) {
		return (atan((temp-400)/40) + Const::pi/2) / Const::pi;
	};*/
	//SmoothedLocalStepFunction step(600, 100);
	physCont->ionizationOverheatingInstFunc = SmoothedLocalStepFunction(1000, 50);

	physCont->linkEta = 1e-5; // 1e-4; // 1e-5;
	physCont->linkBeta = 1e4;
	Vector<3> externalField{0.0, 0.0, 0.2e6};
	physCont->setExternalConstField(externalField);


	timeIter->setTime(0.0);
    timeIter->setStep(1e-8);
    //timeIter->setStepBounds(0.0000000002, 0.0000002);
    timeIter->setStepBounds(0.0, 1e-7);
	timeIter->continiousIterParameters().autoStepAdjustment = true;
	timeIter->setStopTime(1e-1);
}

void Modeller::initParameters1(ElectrostaticPhysicalContext* physCont, TimeIterator* timeIter)
{
	physCont->nodeRadiusConductivity = 0.03;
	physCont->linkEta = 2e-5; // 1e-5;
    physCont->linkBeta = 3e4;
    //physCont->linkBeta = 1e5;
	physCont->initialConductivity = 1e-5;
	physCont->minimalConductivity = physCont->initialConductivity * 0.95;
	Vector<3> externalField{0.0, 0.0, 0.3e6};
}

std::string Modeller::getTimeStr()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
	return oss.str();
}
