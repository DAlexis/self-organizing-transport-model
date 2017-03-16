#include "sotm/base/parameters.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <iostream>

using namespace std;

using namespace sotm;

ParametersGroup::ParametersGroup(const std::string& name) :
		m_name(name)
{
}

void ParametersGroup::add(IAnyTypeParameter& par)
{
	m_parameters[par.name()] = &par;
}

void ParametersGroup::load(const std::string& filename)
{
	using namespace boost::property_tree;
	ptree pt;
	try {
		boost::property_tree::ini_parser::read_ini(filename, pt);
		auto pars = pt.get_child("Parameters");
		for (ptree::iterator it = pars.begin(); it != pars.end(); it++)
		{
			auto jt = m_parameters.find(it->first);
			if (jt != m_parameters.end())
			{
				jt->second->setFromString(it->second.data());
			}
		}
	}
	catch(boost::property_tree::ini_parser::ini_parser_error &exception)
	{
		throw std::runtime_error(std::string("Parsing error in ") + exception.filename()
				+ ":" + std::to_string(exception.line()) + " - " + exception.message());
	}
	catch(boost::property_tree::ptree_error &exception)
	{
		throw std::runtime_error(std::string("Parsing error in ") + filename + ": " + exception.what());
	}
	catch(...)
	{
		throw std::runtime_error(std::string("Unknown parsing error in ") + filename);
	}
}

void ParametersGroup::save(const std::string& filename)
{
	ofstream outputFile(filename.c_str(), std::ios::out);
	if (!outputFile.is_open())
	{
		cerr << "ERROR: Cannot open file " << filename << " to write current parameters!" << endl;
		return;
	}

	outputFile << *this;
}

std::ostream& sotm::operator<<(std::ostream& stream, const ParametersGroup& pg)
{
	stream << "[" << pg.m_name << "]" << std::endl;
	for (auto &it : pg.m_parameters)
	{
		stream << it.first << " = " << it.second->toString() << std::endl;
	}
	return stream;
}

