#include <iostream>						// Provides standard input/output stream objects (i.e. cout)
#include <fstream>						// Provides stream class to  read and write files (i.e. ofstream)

#include "ioManager.h"
#include "Cat_reactor.h"				// Header file that contains this 

bool Cat_reactor::controlSteps_SS(double maxRates, double maxDiff) {
	// declare necessary variables
	double* lastCov = new double[nrSurfSpec];
	bool steadyState = false;
	int check = 0;
	dt = initStep;
	
	// initialize output for every timestep if applicable
	string outputName = "";
	if (ioManager::getNameOutputOpts() == ioManager::FILENAME_TIMESTAMP) {
		outputName = "ss_" + to_string(pot) + "_" + timeStamp;
	}
	ofstream f2 = ioManager::createOutput(outputName, individualOutput);
	ioManager::writeHeader(this, f2, "");
	
	
	// calculate system state for t=0
	t = 0;
	lastChangeTime = -1;
	evaluateStep();
	ioManager::writeStep(this, f2, "");
	double t_lastWritten = t;

	// start timestepping
	while (!steadyState) {
		// TAKE A TIMESTEP
		check = timestep(dt);

		// if internal timestepper/solver failed, abort 
		if (check < 0) {
			ioManager::log("Aborting simulation at t = " + to_string(t) + "s", 0, 1);
			break;
		}

		// if successful, increase t to current state and save last step's coverages
		t += dt;
		for (int i = 0; i < nrSurfSpec; i++) {
			lastCov[i] = cov[i];
		}

		// evaluate and write current timestep response
		evaluateStep();
		if ((t - t_lastWritten) < initStep && (t - t_lastWritten) < maxStep / 10) {
			// Do not write this step so CSV files do not become too big
		}
		else {
			ioManager::writeStep(this, f2, "");
			t_lastWritten = t;
		}
		
		steadyState = checkSteadyState(true);

		if (steadyState) {
			ioManager::log("Steady state reached at t = " + to_string(t) + " s", 0, 1);
			ioManager::log("============================", 0, 1);
			//cout << "Steady state reached at t = " << t << " s" << endl;
			//cout << "============================" << endl;
			lastChangeTime = t;
		}

		if (t > ssTimeMax) {
			ioManager::log("Steady state not reached after " + to_string(ssTimeMax) + " s ... aborting now", 0, 1);
			ioManager::log("============================", 0, 1);
			break;
		}

		// CALCULATE NEXT TIMESTEP
		// if dt could be taken in one go, try bigger timestep next time
		// careful to not diverge with giant timesteps: only x2 at a time and no step > 10s
		if (check == 0 && dt < maxStep) {
			dt *= 2;
		}
		// if dt could not be taken in one go, try smaller timestep next time
		if (check > 0) {
			for (int i = 0; i < check; i++) {
				dt /= 5;
			}
		}

	}
	f2.close();
	return steadyState;
}