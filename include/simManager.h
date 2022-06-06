#pragma once
#include "varManager.h"
#include "ioManager.h"
#include "Cat_reactor.h"

using namespace std;

class simManager
{
	public:
		static void initModel();
		static void nextSim();
		static void runSim();
	private:
		static ofstream f1;

		// general variables
		static varManager variable;
		static int i_max;
		static string phi;
		static string t_end;
		static string dt_max;
		static string dt_min;
		static string dt_init;
		static string timeStamp;

		static Cat_reactor* reactor;

		// variables of current iteration
		static int i;
		static string var;
		static varManager potential;
		static double endTime;
		static double maxTimeStep;
		static double minTimeStep;
		static double initTimeStep;

		static bool cumulatedOutput;
};

