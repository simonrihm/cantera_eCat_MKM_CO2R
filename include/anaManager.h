#pragma once

#include <string>
#include "Cat_reactor.h"

class anaManager
{
public:
	static void analyseMolecule(int* nr_atoms, string molecule);
	static int redState(string molecule);
	static double getCurrDens(string species, double netProd);
private:
	const static int C = 0; 
	const static int H = 1;
	const static int O = 2;
	const static int Cu = 3;
	const static int charge = 4;
};

