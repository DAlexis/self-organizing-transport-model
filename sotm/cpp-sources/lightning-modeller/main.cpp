#include "modeller.hpp"

#include <iostream>

using namespace std;
using namespace sotm;

int main(int argc, char** argv)
{
	Modeller m;
	if (m.parseCmdLineArgs(argc, argv))
		m.run();
	return 0;
}
