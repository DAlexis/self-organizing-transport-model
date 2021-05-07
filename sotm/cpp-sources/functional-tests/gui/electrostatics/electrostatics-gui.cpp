#include <iostream>

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/electrostatics/electrostatics.hpp"
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

    c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new ElectrostaticPhysicalContext()));
    ElectrostaticPhysicalContext* physCont = static_cast<ElectrostaticPhysicalContext*>(c.physicalContext());

    c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new ElectrostaticNodePayloadFactory(*physCont)));
    c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new ElectrostaticLinkPayloadFactory(*physCont)));


	c.parallelSettings.parallelContiniousIteration.calculateSecondaryValues = true;
	c.parallelSettings.parallelBifurcationIteration.prepareBifurcation = true;



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

	physCont->connectionCriticalField = 0.3e6;
	physCont->initialConductivity = 1e-5;
	physCont->minimalConductivity = physCont->initialConductivity * 0.95;

	physCont->chargeScaler.fixValue(0.0, 0.5);
	physCont->chargeColorMapper.setBlueRed();

	physCont->conductivityScaler.setScale(Scaler::Scale::log);
	physCont->conductivityColorMapper.setGreenYellow();
    physCont->linkEtaDefault = 1e-5; // 1e-4; // 1e-5;

	//Vector<3> externalField{0.0, 0.0, 0.6e6};
	StaticVector<3> externalField{0.0, 0.0, 0.2e6};
	//physCont->externalConstField = externalField;
	FieldLinearScalar<3> externalPotential(-0.2e6, {0.0, 0.0, 1.0});
	physCont->externalPotential = &externalPotential;

	ElectrostaticNodePayload::setChargeColorLimits(0.0, 0.0);

	{ // Scope to remove pointers

		/// Building initial tree
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, StaticVector<3>(0.0, 0.0, -0.2));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, StaticVector<3>(0.0, 0.0, 0.2));

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
	iter.setStep(0.00000001);
	iter.setStopTime(1e-4);
	//iter.run();

	// Running GUI
	GUI gui(&c, &iter);
	gui.renderPreferences()->lineWidth = false;
	gui.setFrameOptions(0.000001, 30);
	gui.run(argc, argv);

	cout << "Destroying graph" << endl;
    c.destroyAll();
	cout << "Exiting" << endl;
	return 0;
}
