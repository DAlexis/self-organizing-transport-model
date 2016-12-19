#include <iostream>

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/electrostatics/electrostatics-simple.hpp"
#include "sotm/time-iter/euler-explicit.hpp"

#include "sotm-gui/gui.hpp"

using namespace std;
using namespace sotm;


int main(int argc, char** argv)
{
	ModelContext c;

	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new ElectrostaticNodePayloadFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new ElectrostaticLinkPayloadFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new ElectrostaticPhysicalContext()));

	static_cast<ElectrostaticPhysicalContext*>(c.physicalContext())->setDischargeFunc(
			[](double E) -> double
			{
				double Eabs = fabs(E);
				if (Eabs > 2e6)
					return (Eabs - 2e6)/2e6 * 100;
				return 0.0;
			}
	);

	ElectrostaticNodePayload::setChargeColorLimits(0.0, 50e-6);

	{ // Scope to remove pointers

		/// Building initial tree
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Point<3>(0.0, 0.0, 0.0));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Point<3>(1.0, 1.0, 1.0));

		PtrWrap<Node> n3 = PtrWrap<Node>::make(&c, Point<3>(2.0, 1.0, 1.0));
		PtrWrap<Node> n4 = PtrWrap<Node>::make(&c, Point<3>(1.0, 2.0, 1.0));
		PtrWrap<Node> n5 = PtrWrap<Node>::make(&c, Point<3>(1.0, 1.0, 2.0));

		PtrWrap<Link> l = PtrWrap<Link>::make(&c);
		PtrWrap<Link> l1 = PtrWrap<Link>::make(&c);
		PtrWrap<Link> l2 = PtrWrap<Link>::make(&c);
		PtrWrap<Link> l3 = PtrWrap<Link>::make(&c);

		l->connect(n1, n2);
		l1->connect(n2, n3);
		l2->connect(n2, n4);
		l3->connect(n2, n5);

		static_cast<ElectrostaticNodePayload*>(n1->payload.get())->setCharge(1e-6);
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
