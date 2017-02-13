#include <iostream>

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/electrostatics/electrostatics-simple.hpp"
#include "sotm/time-iter/euler-explicit.hpp"
#include "sotm/time-iter/runge-kutta.hpp"
#include "sotm/math/random.hpp"
#include "sotm/utils/const.hpp"

#include "sotm-gui/gui.hpp"
#include <tbb/tbb.h>
#include <math.h>

using namespace std;
using namespace sotm;

void initParameters(ElectrostaticPhysicalContext* physCont, TimeIterator* timeIter)
{
	physCont->setDischargeFunc(
			[](double E) -> double
			{
				if (E > 0.5e6) // 20 kV/cm
					return (E - 0.5e6)/2e6 * 4e7;
				if (E < -1e6) // 20 kV/cm
					return (-E - 1e6)/2e6 * 4e7;
				return 0.0;
			}
	);

	physCont->nodeRadius = 0.05;
	physCont->linkRadius = 0.001;
	physCont->branchingStep = 0.3;

	physCont->connectionCriticalField = 0.3e6;
	physCont->connectionMaximalDist = physCont->branchingStep.get();

	physCont->initialConductivity = 1e-5;
	physCont->initialConductivity = 1e-2;
	physCont->minimalConductivity = physCont->initialConductivity * 0.95;

	physCont->conductivityLimit = 1e0;
	physCont->ionizationOverheatingInstFunc = [](double temp) {
		return (atan((temp-400)/400) + Const::pi/2) / Const::pi;
	};

	physCont->linkEta = 1e-5; // 1e-4; // 1e-5;
	physCont->linkBeta = 1e4;
	Vector<3> externalField{0.0, 0.0, 0.2e6};
	physCont->setExternalConstField(externalField);


	timeIter->setTime(0.0);
	timeIter->setStep(0.00000002);
	timeIter->setStepBounds(0.000000002, 0.0000002);
	timeIter->continiousIterParameters().autoStepAdjustment = true;
	timeIter->setStopTime(1e-3);
}

void initParameters1(ElectrostaticPhysicalContext* physCont, TimeIterator* timeIter)
{
	//physCont->linkBeta = 0;
	physCont->nodeRadius = 0.03;
	physCont->linkEta = 1e-4; // 1e-5;
	physCont->initialConductivity = 1e-3;
	physCont->minimalConductivity = physCont->initialConductivity * 0.95;
	Vector<3> externalField{0.0, 0.0, 0.3e6};
}

int main(int argc, char** argv)
{
	//tbb::task_scheduler_init init(2);
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

	physCont->chargeScaler.fixValue(0.0, 0.5);
	physCont->chargeColorMapper.setBlueRed();

	physCont->conductivityScaler.setScale(Scaler::Scale::log);
	physCont->conductivityColorMapper.setGreenYellow();

	{ // Scope to remove pointers

		/// Building initial tree
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Vector<3>(0.0, 0.0, -0.2));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Vector<3>(0.0, 0.0, 0.2));

		PtrWrap<Link> l = PtrWrap<Link>::make(&c);

		l->connect(n1, n2);

		//static_cast<ElectrostaticNodePayload*>(n1->payload.get())->setCharge(4e-6);
		//static_cast<ElectrostaticNodePayload*>(n2->payload.get())->setCharge(4e-6);
	}

	c.initAllPhysicalPayloads();

	// Time iteration

	//iter.run();

	// Running GUI
	GUI gui(&c, &iter);
	gui.renderPreferences()->lineWidth = false;
	gui.setFrameOptions(0.000001, 30);
	gui.run(argc, argv);

	cout << "Destroying graph" << endl;
	ElectrostaticPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);

	cout << "Exiting" << endl;
	return 0;
}
