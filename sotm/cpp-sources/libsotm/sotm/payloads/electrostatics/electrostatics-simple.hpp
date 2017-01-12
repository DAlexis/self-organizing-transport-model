/*
 * electrostatics-simple.hpp
 *
 *  Created on: 13 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SIMPLE_HPP_
#define LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SIMPLE_HPP_

#include "sotm/base/physical-payload.hpp"
#include "sotm/math/integration.hpp"
#include <memory>

namespace sotm
{

class ElectrostaticPhysicalContext : public PhysicalContextBase
{
friend class ElectrostaticNodePayload;
public:
	void destroyGraph();
	bool readyToDestroy();

	virtual void calculateSecondaryValues() override;
	virtual void calculateRHS(double time) override;
	virtual void addRHSToDelta(double m) override;
	virtual void makeSubIteration(double dt) override;
	virtual void step() override;

	void doBifurcation(double time, double dt) { UNUSED_ARG(time); UNUSED_ARG(dt); }

	void setDischargeFunc(Function1D func);
	void setExternalConstField(Vector<3> field);

	void getElectricField(const Vector<3>& point, Vector<3>& outField, double& outPotential, const Node* excludeNode = nullptr);

	static inline ElectrostaticPhysicalContext* cast(IPhysicalContext* context)
	{
		return static_cast<ElectrostaticPhysicalContext*>(context);
	}

	static inline const ElectrostaticPhysicalContext* cast(const IPhysicalContext* context)
	{
		return static_cast<const ElectrostaticPhysicalContext*>(context);
	}

	double airTemperature = 300;

private:
	bool m_readyToDestroy = false;
	Function1D m_dischargeProb{zero};
	std::unique_ptr<DefinedIntegral> m_integralOfProb;
	Vector<3> m_externalConstField;
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
	void getBranchingParameters(double time, double dt, BranchingParameters& branchingParameters) override;

	void getColor(double* rgb) override;
	double getSize() override;
	std::string getFollowerText() override;

	void setCharge(double charge);

	static void setChargeColorLimits(double chargeMin, double chargeMax);
	// Parameters
	double radius = 0.13;
	double branchProbeStep = 0.001;

	// Primary
	Variable charge;

	// Secondary
	double phi = 0; // Electrostatic potential
	Vector<3> externalField; // Electric field from other nodes

private:
	double calculateBranchLen(
			const Vector<3>& startPoint,
			const Vector<3>& direction,
			double eDiffMax,
			double lenMax
	);

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

	void calculateHeatnessByTemperature(double temp);

	// Parameters
	double diameter = 0.01; // 1cm
	//constexpr static Cp = 1.0

	// Primary
	double heatness = 0.0;

	// Secondary
	double temperature = 0.0;
	double current = 0;
	double conductivity = 300e-13;
};

SOTM_QUICK_NPF(ElectrostaticNodePayload, ElectrostaticNodePayloadFactory);
SOTM_QUICK_LPF(ElectrostaticLinkPayload, ElectrostaticLinkPayloadFactory);

}







#endif /* LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SIMPLE_HPP_ */
