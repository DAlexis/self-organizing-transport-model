/*
 * electrostatics-simple.hpp
 *
 *  Created on: 13 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SIMPLE_HPP_
#define LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SIMPLE_HPP_

#include "sotm/base/physical-payload.hpp"
#include "sotm/base/parameters.hpp"
#include "sotm/math/integration.hpp"
#include "sotm/output/variables.hpp"

#include <memory>

namespace sotm
{

class ElectrostaticPhysicalContext : public PhysicalContextBase
{
friend class ElectrostaticNodePayload;
public:
	void destroyGraph();
	bool readyToDestroy();

	void calculateSecondaryValues(double time) override;
	void calculateRHS(double time) override;
	void addRHSToDelta(double m) override;
	void makeSubIteration(double dt) override;
	void step() override;

	void doBifurcation(double time, double dt) { UNUSED_ARG(time); UNUSED_ARG(dt); }

	void setDischargeFunc(Function1D func);
	void setExternalConstField(Vector<3> field);

	void getElectricField(const Vector<3>& point, Vector<3>& outField, double& outPotential, const Node* excludeNode = nullptr);

	bool testConnection(Node* n1, Node* n2);

	static inline ElectrostaticPhysicalContext* cast(IPhysicalContext* context)
	{
		return static_cast<ElectrostaticPhysicalContext*>(context);
	}

	static inline const ElectrostaticPhysicalContext* cast(const IPhysicalContext* context)
	{
		return static_cast<const ElectrostaticPhysicalContext*>(context);
	}

	ParametersGroup parameters;

	Parameter<double> airTemperature{parameters, "airTemperature", 300};

	Parameter<double> branchingStep{parameters, "branchingStep"};

	Parameter<bool>   smartBranching{parameters, "smartBranching", false};
	Parameter<double> smartBranchingEDiff{parameters, "smartBranchingEDiff", 0.5};
	Parameter<double> smartBranchingMaxLen{parameters, "smartBranchingMaxLen", 0.5};

	Parameter<double> initialConductivity{parameters, "initialConductivity"};
	Parameter<double> minimalConductivity{parameters, "minimalConductivity"};

	Parameter<double> connectionCriticalField{parameters, "connectionCriticalField"};
	Parameter<double> connectionMaximalDist{parameters, "connectionMaximalDist"};

	Parameter<double> nodeRadius{parameters, "nodeRadius"};
	Parameter<double> linkRadius{parameters, "linkRadius"};

	Parameter<double> linkEta{parameters, "linkEta"};
	Parameter<double> linkBeta{parameters, "linkBeta"};

	Function1D ionizationOverheatingInstFunc{zero};
	Parameter<double> conductivityLimit{parameters, "conductivityLimit"};

	Scaler chargeScaler;
	LinearGradientColorMapper chargeColorMapper;

	Scaler conductivityScaler;
	LinearGradientColorMapper conductivityColorMapper;

private:
	bool m_readyToDestroy = false;
	Function1D m_dischargeProb{zero};
	Function1D m_IOInstFunc{zero};
	std::unique_ptr<DefinedIntegral> m_integralOfProb;
	Vector<3> m_externalConstField;
};

class ElectrostaticNodePayload : public NodePayloadBase
{
public:
	ElectrostaticNodePayload(PhysicalPayloadsRegister* reg, Node* node);

	SOTM_INLINE ElectrostaticPhysicalContext* context() { return static_cast<ElectrostaticPhysicalContext*>(node->physicalContext()); }

	void clearSubiteration() override final;
	void calculateSecondaryValues(double time) override;
	void calculateRHS(double time) override;
	void addRHSToDelta(double m) override;
	void makeSubIteration(double dt) override;
	void step() override;
	double getMinimalStepsCount() override;

	void prepareBifurcation(double time, double dt) override;
	void doBifurcation(double time, double dt) override;
	void init() override;
	void getBranchingParameters(double time, double dt, BranchingParameters& branchingParameters) override;

	void getColor(double* rgb) override;
	double getSize() override;
	std::string getFollowerText() override;

	void setCharge(double charge);

	static void setChargeColorLimits(double chargeMin, double chargeMax);
	// Parameters
	double branchProbeStep = 0.001;

	// Primary
	Variable charge;

	// Secondary
	double phi = 0; // Electrostatic potential
	Vector<3> externalField; // Electric field from other nodes

private:
	void calculateExtFieldAndPhi();
	void findTargetToConnect();
	void connectToTarget();
	double calculateBranchLen(
			const Vector<3>& startPoint,
			const Vector<3>& direction,
			double eDiffMax,
			double lenMax
	);

	static double chargeMin;
	static double chargeMax;

	Node* m_connectTo = nullptr;
};

class ElectrostaticLinkPayload : public LinkPayloadBase
{
public:
	ElectrostaticLinkPayload(PhysicalPayloadsRegister* reg, Link* link);

	SOTM_INLINE ElectrostaticPhysicalContext* context() { return static_cast<ElectrostaticPhysicalContext*>(link->physicalContext()); }

	void clearSubiteration() override final;
	void calculateSecondaryValues(double time) override;
	void calculateRHS(double time) override;
	void addRHSToDelta(double m) override;
	void makeSubIteration(double dt) override;
	void step() override;
	double getMinimalStepsCount() override;

	void doBifurcation(double time, double dt) override;

	void init() override;
	void getColor(double* rgb) override;
	double getSize() override;
	std::string getFollowerText() override;
	void getParametersVector(double* parameters) override;


	void setTemperature(double temp);
	double getTemperature();
	double getTotalConductivity();
	double getIOIEffectiveCondictivity();
	double getCurrent();
	double getVoltage();

	double getHeatCapacity();

	// Parameters
	double radius = 0.01; // 1cm

	// Primary
	Variable conductivity; // Simens
	Variable temperature;

	// Secondary
	//double current = 0;
};

SOTM_QUICK_NPF(ElectrostaticNodePayload, ElectrostaticNodePayloadFactory);
SOTM_QUICK_LPF(ElectrostaticLinkPayload, ElectrostaticLinkPayloadFactory);

}


#endif /* LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_SIMPLE_HPP_ */
