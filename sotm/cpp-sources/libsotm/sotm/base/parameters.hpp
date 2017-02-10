/*
 * parameters.hpp
 *
 *  Created on: 8 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_BASE_PARAMETERS_HPP_
#define LIBSOTM_SOTM_BASE_PARAMETERS_HPP_

#include "sotm/utils/macros.hpp"
#include "sotm/utils/assert.hpp"
#include "sotm/utils/string.hpp"

#include <string>
#include <map>

namespace sotm {

class IAnyTypeParameter
{
public:
	virtual ~IAnyTypeParameter() {}
	virtual void setFromString(const std::string& str) = 0;
	virtual const std::string& name() = 0;
};

class ParametersGroup
{
public:
	void add(IAnyTypeParameter& par);
	void load(const std::string& filename);
	void save(const std::string& filename);

private:
	std::map<const std::string, IAnyTypeParameter*> m_parameters;
};

template<typename T>
struct Parameter : public IAnyTypeParameter
{
	Parameter(ParametersGroup& group, const char* name, T initValue) :
		m_value(initValue),
		m_name(name)
#ifdef DEBUG
		, m_isInitialized(true)
#endif
	{ group.add(*this); }

	Parameter(ParametersGroup& group, const char* name) :
		m_value(0.0),
		m_name(name)
#ifdef DEBUG
		, m_isInitialized(false)
#endif
	{  group.add(*this); }

	Parameter(const char* name, T initValue) :
		m_value(initValue),
		m_name(name)
#ifdef DEBUG
		, m_isInitialized(true)
#endif
	{ }

	Parameter(const char* name) :
		m_value(0.0),
		m_name(name)
#ifdef DEBUG
		, m_isInitialized(false)
#endif
	{ }

	SOTM_INLINE T operator=(T newValue)
	{
#ifdef DEBUG
		m_isInitialized = true;
#endif
		return m_value = newValue;
	}

	SOTM_INLINE T& get()
	{
		return m_value;
	}

	SOTM_INLINE operator T() const
	{
		ASSERT(m_isInitialized, std::string("Parameter ") + m_name + " usage without initialization!");
		return m_value;
	}

	const std::string& name() override { return m_name; }

	void setFromString(const std::string& str) override
	{
		m_value = parseString<T>(str);
#ifdef DEBUG
		m_isInitialized = true;
#endif
	}

private:
	T m_value;
	const std::string m_name;
#ifdef DEBUG
	bool m_isInitialized;
#endif
};

#define NP(group, parameterName)         Parameter<double> parameterName{group, STRINGIFICATE(parameterName)}
#define NPV(group, parameterName, value) Parameter<double> parameterName{group, STRINGIFICATE(parameterName), value}

} // namespace sotm

#endif /* LIBSOTM_SOTM_BASE_PARAMETERS_HPP_ */
