/*
 * main.cpp
 *
 *  Created on: 22 сент. 2016 г.
 *      Author: dalexies
 */

#include <iostream>

#include "sotm-gui/gui.hpp"

using namespace std;
using namespace sotm;


int main(int argc, char** argv)
{
	ModelContext c;
	GUI gui(&c);
	gui.run(argc, argv);
	test();
	cout << "Hello, test" << endl;
	return 0;
}
