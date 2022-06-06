#include "simManager.h"
#include "Cat_reactor.h"
#include "varManager.h"

// general variables
varManager simManager::variable;
Cat_reactor* simManager::reactor;
int simManager::i_max;

// variables of current iteration
string simManager::var;
string simManager::phi;
int simManager::i;
varManager simManager::potential;
string simManager::t_end;
string simManager::dt_max;
string simManager::dt_min;
string simManager::dt_init;
string simManager::timeStamp;
double simManager::endTime;
double simManager::maxTimeStep;
double simManager::minTimeStep;
double simManager::initTimeStep;
ofstream simManager::f1;
bool simManager::cumulatedOutput;

void simManager::initModel() {
	// no of steps and general parameters
	i = 0;
	i_max = ioManager::getNoStep();
	timeStamp = ioManager::createTimeStamp();
	string xmlFile = ioManager::getXml();
	if (ioManager::getCopyMechanismOpts()) {
		string xmlCopy = ioManager::getOutputPath() + "\mechanism_" + timeStamp + ".xml";
		ioManager::copyFile(xmlFile, xmlCopy);
	}
	if (ioManager::getTxtLogOpts()) {
		string logFile = ioManager::getOutputPath() + "\log_" + timeStamp + ".txt";
		ioManager::createLog(logFile);
	}
	reactor = new Cat_reactor(xmlFile);

	// set variable from input
	variable = varManager();
	variable.setLim(0.0, i_max*1.0);
	variable.readFun(ioManager::getVar());
	
	// set other parameters from input
	phi = ioManager::getPhi();
	t_end = ioManager::getEndTime();
	dt_max = ioManager::getMaxTimeStep();
	dt_min = ioManager::getMinTimeStep();
	dt_init = ioManager::getInitTimeStep();
	
	// check if cumulated results should be printed
	int tempOO = ioManager::getFileOutputOpts();
	cumulatedOutput = (tempOO==ioManager::OUTPUT_ALL || tempOO == ioManager::OUTPUT_CUMULATED);
	
	/*if (cumulatedOutput && i_max <= 0) {
		cumulatedOutput = false;
		cout << "No cumulated outputs because number of steps is not greater than 1!" << endl;
	}*/

	// initialize output for cumulated results
	string outputName = "";
	if (ioManager::getNameOutputOpts() == ioManager::FILENAME_TIMESTAMP) {
		outputName = "cumulatedResults_" + timeStamp;
	}
	ioManager::log("Initialize simulation runs...", 0, 1);
	f1 = ioManager::createOutput(outputName, cumulatedOutput);
	ioManager::writeHeader(reactor, f1, "success");
	
}

void simManager::nextSim() {	// prepare next simulation
	// get current value of the general variable
	var = to_string(variable.varFun(i*1.0));
	// initialize potential=f(t) for this simulation
	potential = varManager();

	// set max time step
	/*int dtMaxVarPos = dt_max.find("var");
	if (dtMaxVarPos != dt_max.npos) {
		dt_max.replace(dtMaxVarPos, 3, var);
	}*/
	maxTimeStep = stof(dt_max);

	// set min time step
	/*int dtMinVarPos = dt_min.find("var");
	if (dtMinVarPos != dt_min.npos) {
		dt_min.replace(dtMinVarPos, 3, var);
	}*/
	minTimeStep = stof(dt_min);

	// set initial time step
	initTimeStep = stof(dt_init);
	if (initTimeStep <= 0.0) {
		initTimeStep = maxTimeStep / 10;
	}
	reactor->setStepping(initTimeStep, maxTimeStep, minTimeStep);

	// set end time
	int tEndVarPos = t_end.find("var");
	if (tEndVarPos != t_end.npos) {
		t_end.replace(tEndVarPos, 3, var);
	}
	endTime = stof(t_end);
	potential.setLim(0.0, abs(endTime));

	// set potential function
	string tempPhi = phi;
	int phiVarPos = tempPhi.find("var");
	if (phiVarPos != tempPhi.npos) {
		tempPhi.replace(phiVarPos, 3, var);
	}
	potential.readFun(tempPhi);
	
}

void simManager::runSim() {
	bool check = false;

	while (i <= i_max) {
		// prepare next simulation
		ioManager::log("Preparing simulation run nr. " + to_string(i) + " ... ", 0, 1); //cout << endl << "Preparing simulation run nr. " << i << " ... " << endl;
		nextSim();	

		// Do one simulation
		if (endTime < 0) {
			// negative endTime means steady-state
			if (potential.getFunMode() != varManager::FUNMODE_CONSTANT) {
				ioManager::log("Steady-state calculations can only be done for constant potential! Step skipped", 0, 1); //cout << "Steady-state calculations can only be done for constant potential! Step skipped" << endl;
				continue;
			}
			else {
				// Simulate steady-state response
				double pot = potential.varFun(1);
				reactor->setPot(pot);
				ioManager::log("Solving steady-state problem for phi=" + to_string(pot) + " V ...", 0, 1); //cout << "Solving steady-state problem for phi=" << pot << " V ..." << endl;
				check = reactor->controlSteps_SS(1e-5, 1e-6);
			}
		}
		else {
			// Simulate transient response
			ioManager::log("Starting transient simulation...", 0, 1); //cout << "Starting transient simulation..." << endl;
			check = reactor->controlSteps_t0(endTime, &potential);
		}

		// write last timestep of simulation result
		if (cumulatedOutput) {
			ioManager::writeStep(reactor, f1, to_string(check));
			reactor->reset();
		}
		i++;
	}
	delete reactor;
}