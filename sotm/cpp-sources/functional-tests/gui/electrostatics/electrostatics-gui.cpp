#include <iostream>

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/electrostatics/electrostatics-simple.hpp"
#include "sotm/time-iter/euler-explicit.hpp"
#include "sotm/math/random.hpp"

#include "sotm-gui/gui.hpp"

using namespace std;
using namespace sotm;


int main(int argc, char** argv)
{
	Random::randomize(0);
	ModelContext c;

	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new ElectrostaticNodePayloadFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new ElectrostaticLinkPayloadFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new ElectrostaticPhysicalContext()));

	ElectrostaticPhysicalContext* physCont = static_cast<ElectrostaticPhysicalContext*>(c.physicalContext());
	/*physCont->setDischargeFunc(
			[](double E) -> double
			{
				double Eabs = fabs(E);
				if (Eabs > 2e6) // 20 kV/cm
					return (Eabs - 2e6)/2e6 * 400;
				return 0.0;
			}
	);
	*/
	physCont->setDischargeFunc(
			[](double E) -> double
			{
				if (E > 1e6) // 20 kV/cm
					return (E - 1e6)/2e6 * 400;
				if (E < -0.5e6) // 20 kV/cm
					return (-E - 0.5e6)/2e6 * 400;
				return 0.0;
			}
	);

	//Vector<3> externalField{0.0, 0.0, 0.6e6};
	Vector<3> externalField{0.0, 0.0, 0.2e6};
	physCont->setExternalConstField(externalField);

	ElectrostaticNodePayload::setChargeColorLimits(0.0, 50e-6);

	{ // Scope to remove pointers

		/// Building initial tree
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Vector<3>(0.0, 0.0, -0.2));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Vector<3>(0.0, 0.0, 0.2));
/*
		PtrWrap<Node> n3 = PtrWrap<Node>::make(&c, Vector<3>(2.0, 1.0, 1.0));
		PtrWrap<Node> n4 = PtrWrap<Node>::make(&c, Vector<3>(1.0, 2.0, 1.0));
		PtrWrap<Node> n5 = PtrWrap<Node>::make(&c, Vector<3>(1.0, 1.0, 2.0));*/

		PtrWrap<Link> l = PtrWrap<Link>::make(&c);
/*		PtrWrap<Link> l1 = PtrWrap<Link>::make(&c);
		PtrWrap<Link> l2 = PtrWrap<Link>::make(&c);
		PtrWrap<Link> l3 = PtrWrap<Link>::make(&c);*/

		l->connect(n1, n2);
		l->payload->init();
/*		l1->connect(n2, n3);
		l2->connect(n2, n4);
		l3->connect(n2, n5);*/

		//static_cast<ElectrostaticNodePayload*>(n1->payload.get())->setCharge(4e-6);
		//static_cast<ElectrostaticNodePayload*>(n2->payload.get())->setCharge(4e-6);
	}

	// Time iteration
	EulerExplicitIterator euler;
	TimeIterator iter(&c, &euler, &c);
	iter.setTime(0.0);
	iter.setStep(0.001);
	iter.setStopTime(1.0);

	// Running GUI
	GUI gui(&c, &iter);
	gui.run(argc, argv);

	cout << "Destroying graph" << endl;
	ElectrostaticPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);

	cout << "Exiting" << endl;
	return 0;
}
