/*
 * field-calculator.hpp
 *
 *  Created on: 1 мар. 2017 г.
 *      Author: dalexies
 */

#ifndef UTILITIES_FIELD_CALCULATOR_FIELD_CALCULATOR_HPP_
#define UTILITIES_FIELD_CALCULATOR_FIELD_CALCULATOR_HPP_

#include "sotm/math/geometry.hpp"

#include <boost/program_options.hpp>

class FieldCalculator
{
public:
	bool parseCmdLineArgs(int argc, char** argv);
	void run();

private:
	bool parsePoint(sotm::StaticVector<3>& v, const std::string& str);

	bool getCornersFromCmdline();
	void fixCorners();
	bool readPoints();
	bool createGrid();

	struct Charge
	{
		sotm::StaticVector<3> pos;
		double charge = 0;
	};

    /// @todo All parsing should be in function parseCmdLineArgs! Remove next line.
	boost::program_options::variables_map m_cmdLineOptions;
	std::vector<Charge> m_charges;
	bool m_hasCorners;
	sotm::StaticVector<3> m_c1, m_c2; // Corners

	size_t m_nx = 0, m_ny = 0, m_nz = 0;
	double m_minDist = 0.0;
    sotm::StaticVector<3> m_externalE;
    bool m_ignoreExternal = false;
	std::vector<double> m_potential;
    std::string m_outputFilenamePrefix;
};


#endif /* UTILITIES_FIELD_CALCULATOR_FIELD_CALCULATOR_HPP_ */
