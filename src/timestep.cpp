#include "Cat_reactor.h"
#include "ioManager.h"
//#include <csignal>

// Do the actual timestepping by calling advanceCoverages
int Cat_reactor::timestep(double delta_t) {

	int loop_counter = 0;
	double sum_dt = 0;
	double dt = delta_t;

	double rTol = relTol;
	double aTol = absTol;
	bool tolerance_variation = true;
	int inner_loop = 0;

	while (sum_dt + dt <= delta_t * (1 + 1e-10)) {
		if (loop_counter >= 20 || dt < minStep || timeoutFlag) {
			if (!timeoutFlag && tolerance_variation && inner_loop < 3) {
				inner_loop++;
				ioManager::log("No convergence reached, varying solver tolerances (" + to_string(inner_loop) + "/3)...", 0, 1);
				switch (inner_loop) {
				case 1:
					rTol = relTol / 10;
					aTol = absTol;
					break;
				case 2:
					rTol = relTol;
					aTol = absTol / 10;
					break;
				case 3:
					rTol = relTol / 10;
					aTol = absTol / 10;
					break;
				}
				dt = delta_t;
				loop_counter = 0;
			}
			else {
				ioManager::log("No convergence reached, aborting...", 0, 1);
				return -1;
			}

		}
		try {
			// set timeout of this function call (current solution: externally via SIGINT)
			iKin->advanceCoverages(delta_t, rTol, aTol, 0.0, 20000, 7);
			sum_dt += dt;
		}
		catch (Cantera::CanteraError& err) {
			dt /= 10;
			loop_counter++;
			ioManager::log("Convergence problems, try 9 intermediate steps with dt = " + to_string(dt) + " s", 0, 1);
		}
	}

	return loop_counter;
}

// Set parameters relevant for timestepping
void Cat_reactor::setStepping(double init, double max, double min) {
	this->initStep = init;
	this->maxStep = max;
	this->minStep = min;
}