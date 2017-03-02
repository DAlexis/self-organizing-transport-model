#include "field-calculator.hpp"

#include <boost/program_options.hpp>
#include <memory>
#include <iostream>

int main(int argc, char** argv)
{
	FieldCalculator c;
	if (c.parseCmdLineArgs(argc, argv))
		c.run();
	return 0;
}
