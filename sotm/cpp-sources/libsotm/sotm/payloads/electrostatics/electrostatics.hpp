/*
 * electrostatics-simple.hpp
 *
 *  Created on: 13 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_HPP_
#define LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_HPP_

#include "sotm/base/physical-payload.hpp"
#include "sotm/base/parameters.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/math/integration.hpp"
#include "sotm/math/field.hpp"
#include "sotm/math/electromagnetic-emission.hpp"
#include "sotm/output/variables.hpp"
#include "sotm/optimizers/coulomb-brute-force.hpp"
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
    void init() override;

    void doBifurcation(double time, double dt) override;

	void setDischargeFunc(Function1D func);
    bool testConnection(const Node* n1, const Node* n2) const;

	static inline ElectrostaticPhysicalContext* cast(IPhysicalContext* context)
	{
		return static_cast<ElectrostaticPhysicalContext*>(context);
	}

	static inline const ElectrostaticPhysicalContext* cast(const IPhysicalContext* context)
	{
		return static_cast<const ElectrostaticPhysicalContext*>(context);
	}

    Parameter<double> airTemperature{300};

    Parameter<double> branchingStep;

    Parameter<bool>   smartBranching{false};
    Parameter<double> smartBranchingEDiff{0.5};
    Parameter<double> smartBranchingMaxLen{0.5};

    Parameter<double> initialConductivity;
    Parameter<double> minimalConductivity;

    Parameter<double> connectionCriticalField;
    Parameter<double> connectionMaximalDist;

    Parameter<double> nodeRadiusConductivityDefault;
    Parameter<double> nodeRadiusBranchingDefault;
    Parameter<double> linkRadius;

    Parameter<double> linkEtaDefault;
    Parameter<double> linkBetaDefault;
    Parameter<double> conductivityLimit;

    Function1D ionizationOverheatingInstFunc{zero};

    /// @todo Remove coloring/scaling functionality outside
	Scaler chargeScaler;
	LinearGradientColorMapper chargeColorMapper;

	Scaler conductivityScaler;
	LinearGradientColorMapper conductivityColorMapper;

	Field<1, 3> *externalPotential = &zeroField;


    std::unique_ptr<IColoumbCalculator> optimizer{
        new CoulombBruteForce(m_model->graphRegister)
    };

    std::unique_ptr<EmissionCounterWithoutLag> emission_counter;

private:
	Function1D m_dischargeProb{zero};
	Function1D m_IOInstFunc{zero};
	std::unique_ptr<DefinedIntegral> m_integralOfProb;
	static FieldScalarZero<3> zeroField;
};

class ElectrostaticNodePayload : public NodePayloadBase
{
public:
    ElectrostaticNodePayload(PhysicalPayloadsRegister* reg, Node* node, double nodeRadiusConductivity, double nodeRadiusBranching);

    ElectrostaticPhysicalContext* context() { return static_cast<ElectrostaticPhysicalContext*>(node->physicalContext()); }
    const ElectrostaticPhysicalContext* context() const { return static_cast<const ElectrostaticPhysicalContext*>(node->physicalContext()); }

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
	void getParametersVector(double* parameters) override;

	void setCharge(double charge);

	static void setChargeColorLimits(double chargeMin, double chargeMax);
	// Parameters
    /// For smart branching: dynamic branch len
	double branchProbeStep = 0.001;

    double nodeRadiusBranching;
    double nodeRadiusConductivity;

	// Primary
	Variable charge;

	// Secondary
	double phi = 0; // Electrostatic potential
    StaticVector<3> externalField; // Electric field from other nodes

    // For charge field computations
    std::shared_ptr<CoulombNodeBase> coulombNode{
        static_cast<ElectrostaticPhysicalContext*>(node->physicalContext())->optimizer->makeNode(
            charge.current,
            *node
        )
    };

    static double etaFromCriticalField(double criticalFeild, double beta); // Move this to context
private:
	void calculateExtFieldAndPhi();
    Node* findTargetToConnectByMeanField() const;
	void connectToTarget(Node* connectTo);
	double calculateBranchLen(
            const StaticVector<3>& startPoint,
            const StaticVector<3>& direction,
			double eDiffMax,
			double lenMax
	);

	static double chargeMin;
	static double chargeMax;
};

class ElectrostaticLinkPayload : public LinkPayloadBase
{
public:
    ElectrostaticLinkPayload(PhysicalPayloadsRegister* reg, Link* link, double linkEta, double linkBeta);

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

    double linkEta;
    double linkBeta;

	// Primary
	Variable conductivity; // Simens
	Variable temperature;

    double lastCurrent = 0;
    double currentDerivative = 0;
	// Secondary
	//double current = 0;
    StaticVector<3> center;
    StaticVector<3> direction;
};

class ElectrostaticNodePayloadFactory : public INodePayloadFactory
{
public:
    ElectrostaticNodePayloadFactory(ElectrostaticPhysicalContext& context);
    NodePayloadBase* create(PhysicalPayloadsRegister* reg, Node* node) override final;
private:
    ElectrostaticPhysicalContext& m_context;
};

class ElectrostaticLinkPayloadFactory : public ILinkPayloadFactory
{
public:
    ElectrostaticLinkPayloadFactory(ElectrostaticPhysicalContext& context);
    LinkPayloadBase* create(PhysicalPayloadsRegister* reg, Link* link) override final;

private:
    ElectrostaticPhysicalContext& m_context;
};

}


#endif /* LIBSOTM_SOTM_PAYLOADS_ELECTROSTATICS_ELECTROSTATICS_HPP_ */
