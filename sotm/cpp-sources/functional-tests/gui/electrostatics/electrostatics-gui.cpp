#include <iostream>

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/electrostatics/electrostatics-simple.hpp"
#include "sotm/time-iter/euler-explicit.hpp"
#include "sotm/time-iter/runge-kutta.hpp"
#include "sotm/math/random.hpp"

#include "sotm-gui/gui.hpp"
#include <tbb/tbb.h>

using namespace std;
using namespace sotm;


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

	physCont->setDischargeFunc(
		[](double E) -> double
		{
			if (E > 0.5e6) // 20 kV/cm
				return (E - 0.5e6)/2e6 * 4e5;
			if (E < -1e6) // 20 kV/cm
				return (-E - 1e6)/2e6 * 4e5;
			return 0.0;
		}
	);

	physCont->nodeEffectiveRadiusCapacity = 0.05;
	physCont->nodeEffectiveRadiusBranching = 0.13;

	physCont->connectionCriticalField = 0.5e6;
	physCont->initialConductivity = 1e-5;
	physCont->minimalConductivity = physCont->initialConductivity * 0.95;

	physCont->branchingStep = 0.3;

	physCont->linkEta = 1e-5; // 1e-4; // 1e-5;
	physCont->linkBeta = 1e4;


	physCont->chargeScaler.fixValue(0.0, 0.5);
	physCont->chargeColorMapper.setBlueRed();

	physCont->conductivityScaler.setScale(Scaler::Scale::log);
	physCont->conductivityColorMapper.setGreenYellow();


	//Vector<3> externalField{0.0, 0.0, 0.6e6};
	Vector<3> externalField{0.0, 0.0, 0.2e6};
	physCont->setExternalConstField(externalField);

	ElectrostaticNodePayload::setChargeColorLimits(0.0, 0.0);

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
	//EulerExplicitIterator continiousIterator;
	RungeKuttaIterator continiousIterator;
	TimeIterator iter(&c, &continiousIterator, &c);
	iter.setTime(0.0);
	iter.setStep(2e-7);
	iter.setStopTime(1e-2);
	//iter.run();

	// Running GUI
	GUI gui(&c, &iter);
	gui.renderPreferences()->lineWidth = false;
	gui.setFrameOptions(0.0001, 30);
	gui.run(argc, argv);

	cout << "Destroying graph" << endl;
	ElectrostaticPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);

	cout << "Exiting" << endl;
	return 0;
}
