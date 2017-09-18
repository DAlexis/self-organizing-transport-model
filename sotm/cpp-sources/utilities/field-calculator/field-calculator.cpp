#include "field-calculator.hpp"
#include "grid-builder.hpp"

#include "sotm/utils/const.hpp"
#include <tbb/tbb.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool FieldCalculator::parseCmdLineArgs(int argc, char** argv)
{
	namespace po = boost::program_options;
	po::options_description generalOptions("General options");
	generalOptions.add_options()
		("help,h", "Print help message")
        ("input,i", po::value<std::string>(), "Input file name")
        ("nx", po::value<unsigned int>()->default_value(10), "Points per x")
        ("ny", po::value<unsigned int>()->default_value(1), "Points per y")
        ("nz", po::value<unsigned int>()->default_value(70), "Points per z")
		("min-dist,d", po::value<double>()->default_value(0.05), "Charges closer to grid point than this value will be rejected")
        ("ex", po::value<double>()->default_value(0.0), "External field X")
        ("ey", po::value<double>()->default_value(0.0), "External field Y")
        ("ez", po::value<double>()->default_value(0.0), "External field Z")
        ("no-external", "Ignore external field")
        ("output,o", po::value<string>()->default_value("field"), "Output file name prefix")
        ("zone-begin,b", po::value<string>(), "Zone begin coords (x, y, z)")
        ("zone-end,e", po::value<string>(), "Zone end coords (x, y, z)");

	po::options_description allOptions("Allowed options");
	allOptions
		.add(generalOptions);

    try
	{
		po::store(po::parse_command_line(argc, argv, allOptions), m_cmdLineOptions);
		po::notify(m_cmdLineOptions);

		m_nx = m_cmdLineOptions["nx"].as<unsigned int>();
		m_ny = m_cmdLineOptions["ny"].as<unsigned int>();
		m_nz = m_cmdLineOptions["nz"].as<unsigned int>();
		m_minDist = m_cmdLineOptions["min-dist"].as<double>();
        m_externalE[0] = m_cmdLineOptions["ex"].as<double>();
        m_externalE[1] = m_cmdLineOptions["ey"].as<double>();
        m_externalE[2] = m_cmdLineOptions["ez"].as<double>();
        m_ignoreExternal = m_cmdLineOptions.count("no-external") == 0;
        m_outputFilenamePrefix = m_cmdLineOptions["output"].as<string>();
	}
	catch (po::error& e)
	{
		cerr << "Command line parsing error: " << e.what() << endl;
		return false;
	}
    catch(exception& e)
    {
        cerr << "Command line parsing general error: " << e.what() << endl;
        return false;
    }

	if (m_cmdLineOptions.count("help"))
	{
		cout << allOptions << endl;
		return false;
	}

    if (m_cmdLineOptions.count("input") == 0)
	{
		cerr << "File not specified" << endl;
		return false;
	}

	return true;
}

void FieldCalculator::run()
{
	m_hasCorners = getCornersFromCmdline();

	if (!readPoints())
	{
		cerr << "Error wile loading points from file" << endl;
		return;
	}

	createGrid();
}

bool FieldCalculator::parsePoint(sotm::Vector<3>& v, const std::string& str)
{
	std::istringstream iss(str);
	char tmp;
	try
	{
		iss >> tmp;
		if (tmp != '(')
			return false;
		iss >> v[0];
		iss >> tmp;
		if(tmp != ',')
			return false;
		iss >> v[1];
		iss >> tmp;
		if(tmp != ',')
			return false;

		iss >> v[2];
		iss >> tmp;
		if(tmp != ')')
			return false;
	} catch(std::exception& ex)
	{
		cerr << "Invalid point '" << str << "', parsing error: " << ex.what();
		return false;
	}
	return true;
}

bool FieldCalculator::getCornersFromCmdline()
{
	if (m_cmdLineOptions.count("zone-begin") != 0 && m_cmdLineOptions.count("zone-end") != 0)
	{
		return parsePoint(m_c1, m_cmdLineOptions["zone-begin"].as<std::string>())
				&& parsePoint(m_c2, m_cmdLineOptions["zone-end"].as<std::string>());
	}
	return false;
}

void FieldCalculator::fixCorners()
{
	if (m_hasCorners)
	{
		for (int i=0; i<3; i++)
			if (m_c1[i] > m_c2[i])
				std::swap(m_c1[i], m_c2[i]);
	}
}


bool FieldCalculator::readPoints()
{
    std::ifstream file(m_cmdLineOptions["input"].as<std::string>().c_str(), std::ios::in);

	if (!file.is_open())
	{
        cerr << "Cannot open file " << m_cmdLineOptions["input"].as<std::string>() << "!" << endl;
		return false;
	}

	std::string line;
	std::getline(file, line);
	bool firstLine = true;
	while(std::getline(file, line))
	{
		Charge newCharge;
		try
		{
			std::istringstream iss(line);
			char tmp;
			iss >> newCharge.pos[0] >> tmp;
			iss >> newCharge.pos[1] >> tmp;
			iss >> newCharge.pos[2] >> tmp;
			iss >> newCharge.charge;
            //cout << newCharge.pos[0] << " " << newCharge.pos[1] << " " << newCharge.pos[2] << " " << newCharge.charge << endl;
			if (!m_hasCorners)
			{
				// Auto scaling
				for (int i=0; i<3; i++)
				{
					if (firstLine || m_c1[i] > newCharge.pos[i])
						m_c1[i] = newCharge.pos[i];

					if (firstLine || m_c2[i] < newCharge.pos[i])
						m_c2[i] = newCharge.pos[i];
				}
			}
			firstLine = false;
		} catch(std::exception& ex)
		{
			cerr << "File parsing error: " << ex.what() << endl;
			return false;
		}
		m_charges.push_back(newCharge);
	}

	return true;
}

bool FieldCalculator::createGrid()
{
	GridBuilder w(m_nx, m_ny, m_nz,
			m_c1[0], m_c1[1], m_c1[2],
			m_c2[0], m_c2[1], m_c2[2]);

    GridBuilder::ValuePoint *p = w.valuePoints().data();

    tbb::parallel_for( size_t(0), w.valuePoints().size(),
        [this, p]( size_t i ) {
            double potential = 0;

            sotm::Vector<3> &point = p[i].point;

            for (auto &jt : m_charges)
            {
                double dist = (jt.pos - point).len();
                if (dist < m_minDist)
                    continue;
                potential += jt.charge / dist;
            }
            potential *= sotm::Const::Si::k;

            if (!m_ignoreExternal)
                potential += -(m_externalE ^ point);
            p[i].value = potential;
        }
    );

    w.writeFile(m_outputFilenamePrefix);
	return true;
}

