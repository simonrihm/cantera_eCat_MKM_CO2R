#include <iostream>						// Provides standard input/output stream objects (i.e. cout)
#include <fstream>						// Provides stream class to  read and write files (i.e. ofstream)

#include "ioManager.h"
#include "Cat_reactor.h"				// Header file that contains this 
#include "varManager.h"

bool Cat_reactor::controlSteps_t0(double t0, varManager* phiFun) {
	// declare necessary variables
	double* lastCov = new double[nrSurfSpec];
	int check = 0;
	dt = initStep;
	double t_virtual;
	double t00 = t0;
	double dt_avg = dt;

	// calculate endpoint of dynamic stepping, rest will be done statically for averaging
	bool avg_flag = n_avg > 1;
	if (avg_flag) {
		t00 = t0 - t_avg;
		dt_avg = t_avg / n_avg;
		
	}
	vector<Cat_reactor*> *reactorCopies = new vector<Cat_reactor*>;

	// initialize output for every timestep if applicable
	string outputName = "";
	if (ioManager::getNameOutputOpts() == ioManager::FILENAME_TIMESTAMP) {
		outputName = "t0=" + to_string(t0) + "s_" + to_string(phiFun->varFun(0.0)) + "_" + to_string(phiFun->varFun(t0)) + "V_" + timeStamp;
	}
	ofstream f2 = ioManager::createOutput(outputName, individualOutput);
	switch (ioManager::getSampleOutputOpts()) {
	case ioManager::SAMPLE_NONE:
		ioManager::writeHeader(this, f2, "");
		break;
	case ioManager::SAMPLING_T10:
		ioManager::writeHeader(this, f2, "t_sample");
		break;
	}
	
	
	// calculate system state for t=0
	t = 0;
	lastChangeTime = -1.0;
	setPot(phiFun->varFun(t));
	evaluateStep();
	ssFlag = checkSteadyState(false);
	switch (ioManager::getSampleOutputOpts()) {
	case ioManager::SAMPLE_NONE:
		ioManager::writeStep(this, f2, "");
		break;
	case ioManager::SAMPLING_T10:
		ioManager::writeStep(this, f2, to_string(t));
		t_virtual = t + 10;
		break;
	}
	double t_lastWritten = t;
	 

	// start timestepping
	while (t < t0) {
		// TAKE A TIMESTEP
		// calculate current potential and try to take a step
		setPot(phiFun->varFun(t));
		check = timestep(dt);

		// if internal timestepper/solver failed, abort 
		if (check < 0) {
			ioManager::log("Aborting simulation at t = " + to_string(t) + "s", 0, 1);
			break;
		}

		// if successful, increase t to current state - then evaluate and write current timestep response
		ssFlag = checkSteadyState(false);
		
		switch (ioManager::getSampleOutputOpts()) {
		case ioManager::SAMPLE_NONE:
			// without sampling
			t += dt;
			evaluateStep();
			if ((t - t_lastWritten) < initStep && (t - t_lastWritten) < maxStep / 10) {
				// Do not write this step so CSV files do not become too big
			}
			else {
				ioManager::writeStep(this, f2, "");
				t_lastWritten = t;
			}
			break;

		case ioManager::SAMPLING_T10:
			// with sampling every 10 seconds only
			if (t + dt > t_virtual) {
				if (abs(t + dt - t_virtual) < abs(t - t_virtual)) {
					t += dt;
					evaluateStep();
					ioManager::writeStep(this, f2, to_string(t_virtual));
				}
				else {
					ioManager::writeStep(this, f2, to_string(t_virtual));
					t += dt;
					evaluateStep();
				}
				t_lastWritten = t_virtual;
				t_virtual += 10;
			}
			else {
				t += dt;
				evaluateStep();
			}	
			break;
		default:
			throw("error!");
			break;
		}

		if (ssFlag && lastChangeTime == -1 ) {
			lastChangeTime = t;
		}
		else {
			if (!ssFlag && lastChangeTime > 0) {
				lastChangeTime = -1;
			}
		}

		// CALCULATE NEXT TIMESTEP
		// if dt could be taken in one go, try bigger timestep next time (limited by maxStep)
		if (check == 0 && dt < maxStep) {
			dt = max(dt * 2, maxStep);
		}
		// if dt could not be taken in one go, try smaller timestep next time
		if (check > 0) {
			for (int i = 0; i < check; i++) {
				dt /= 5;
			}
		}
		// if the calculated step size would lead to t>t0, then reduce step size to reach t0 exactly
		dt = min(dt, (t00 - t));

		if (!(t < t00)) {
			dt = dt_avg;
			reactorCopies->push_back((new Cat_reactor()));
			reactorCopies->back()->copyResults(this);
		}

	}

	f2.close();

	if (avg_flag && reactorCopies->size() > 0) {
		averageResults(*reactorCopies);
	}
	for (int i = reactorCopies->size() - 1; i >= 0; i--) {
		reactorCopies->back()->terminateCopy();
		delete reactorCopies->back();
		reactorCopies->pop_back();
	}

	return abs(t-t0)/t < 1e-10;		// only return true if t0 was exactly reached (with reasonable accuracy)
}