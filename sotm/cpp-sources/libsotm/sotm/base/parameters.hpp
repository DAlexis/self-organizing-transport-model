
#ifndef LIBSOTM_SOTM_BASE_PARAMETERS_HPP_
#define LIBSOTM_SOTM_BASE_PARAMETERS_HPP_

#include "sotm/utils/macros.hpp"
#include "sotm/utils/assert.hpp"
#include "sotm/utils/string.hpp"

#include <string>
#include <map>
#include <ostream>

namespace sotm {

class IAnyTypeParameter
{
public:
	virtual ~IAnyTypeParameter() {}
	virtual void setFromString(const std::string& str) = 0;
	virtual std::string toString() const = 0;
	virtual const std::string& name() const = 0;
};

class ParametersGroup
{
public:
	ParametersGroup(const std::string& name = "UnknownGroup");
	void add(IAnyTypeParameter& par);
	void load(const std::string& filename);
	void save(const std::string& filename);

	friend std::ostream& operator<<(std::ostream& stream, const ParametersGroup& pg);
private:
	const std::string m_name;
	std::map<const std::string, IAnyTypeParameter*> m_parameters;
};

std::ostream& operator<<(std::ostream& stream, const ParametersGroup& pg);

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

    Parameter(ParametersGroup& group, const char* name = unknown) :
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

    Parameter(T initValue) :
        m_value(initValue),
        m_name(unknown)
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

    Parameter() :
        m_value(0.0),
        m_name(unknown)
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

	const std::string& name() const override { return m_name; }

	void setFromString(const std::string& str) override
	{
		m_value = parseString<T>(str);
#ifdef DEBUG
		m_isInitialized = true;
#endif
	}

	std::string toString() const override
	{
		return std::to_string(m_value);
	}

	Parameter& operator=(const Parameter& right)
	{
#ifdef DEBUG
		m_isInitialized = true;
#endif
		m_value = right.m_value;
		return *this;
	}

private:
    constexpr static const char* unknown = "unknown_parameter";
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
