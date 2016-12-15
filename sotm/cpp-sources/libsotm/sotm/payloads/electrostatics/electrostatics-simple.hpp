/*
 * electrostatics-simple.hpp
 *
 *  Created on: 13 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SIMPLE_HPP_
#define LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SIMPLE_HPP_

#include "sotm/base/physical-payload.hpp"

namespace sotm
{

class ElectrostaticPhysicalContext : public IPhysicalContext
{
public:
	void destroyGraph();
	bool readyToDestroy();

	virtual void calculateSecondaryValues() override;
	virtual void calculateRHS(double time) override;
	virtual void addRHSToDelta(double m) override;
	virtual void makeSubIteration(double dt) override;
	virtual void step() override;

	void doBifurcation(double time, double dt) { UNUSED_ARG(time); UNUSED_ARG(dt); }

	static inline ElectrostaticPhysicalContext* cast(IPhysicalContext* context)
	{
		return static_cast<ElectrostaticPhysicalContext*>(context);
	}

	static inline const ElectrostaticPhysicalContext* cast(const IPhysicalContext* context)
	{
		return static_cast<const ElectrostaticPhysicalContext*>(context);
	}

private:
	bool m_readyToDestroy = false;
};

class ElectrostaticNodePayload : public NodePayloadBase
{
public:
	ElectrostaticNodePayload(PhysicalPayloadsRegister* reg, Node* node);

	void calculateSecondaryValues() override;
	void calculateRHS(double time) override;
	void addRHSToDelta(double m) override;
	void makeSubIteration(double dt) override;

	void step() override;

	void doBifurcation(double time, double dt) override;

	void getColor(double* rgb) override;

	std::string getFollowerText() override;

	void setCharge(double charge);

	static void setChargeColorLimits(double chargeMin, double chargeMax);
	// Parameters
	double radius = 0.01;

	// Primary current
	double charge = 0;

	// Primary previous
	double charge_prev = 0;

	// Secondary
	double phi = 0; // Electrostatic potential
	Point<3> externalField; // Electric field from other nodes

	// RHS
	double charge_rhs = 0;

	// Delta
	double charge_delta = 0;

private:
	static double chargeMin;
	static double chargeMax;
};

class ElectrostaticLinkPayload : public LinkPayloadBase
{
public:
	ElectrostaticLinkPayload(PhysicalPayloadsRegister* reg, Link* link);

	void calculateSecondaryValues() override;
	void calculateRHS(double time) override;
	void addRHSToDelta(double m) override;
	void makeSubIteration(double dt) override;
	void step() override;

	void doBifurcation(double time, double dt) override;

	// Parameters
	double conductivity = 1e-13;

	// Primary

	// Secondary
	double current = 0;

};

SOTM_QUICK_NPF(ElectrostaticNodePayload, ElectrostaticNodePayloadFactory);
SOTM_QUICK_LPF(ElectrostaticLinkPayload, ElectrostaticLinkPayloadFactory);

}







#endif /* LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SIMPLE_HPP_ */
