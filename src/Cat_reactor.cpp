#include <iostream>						// Provides standard input/output stream objects (i.e. cout)
#include <vector>						// Provides class "vector" as container for multiple objects
#include <string>
#include "Cat_reactor.h"				// Header file that contains this 
#include "ioManager.h"

//using namespace std;					// standard namespace is needed e.g. for string 
using namespace Cantera;				// Cantera namespace is needed to access Cantera classes


Cat_reactor::Cat_reactor(string MechFileName)
{
	// PARAMETER DEFINITIONS
	// get current timestamp for output name
	timeStamp = ioManager::createTimeStamp();

	// global variables and parameters
	double temp = 298.15;				// in Kelvin
	double initialPressure = 101325.0;	// total pressure [Pa] at beginning (1 atm)

	// liquid phase auto-ionization concentrations
	double *c_liq{ new double[3] {} };
	double *x_liq{ new double[3] {} };

	// READ INPUT FILE AND SET UP PHASES
	// start reading input file
	ioManager::log("Opening mechanism file " + MechFileName, 0, 1);
	XML_Node* xc = get_XML_File(MechFileName);	// complete input file
	ioManager::log("Reading input XML...", 0, 1);
	allSpecies = getSpecies(*xc);
	nrSpecies = allSpecies.size();
	ioManager::log("Number of species involved = " + to_string(nrSpecies), 0, 1);
	// init phases 
	XML_Node* x[POSSIBLEPHASES_NUMBER];
	ThermoPhase* xTP;
	int j = 0;
	for (int i = 0; i < POSSIBLEPHASES_NUMBER; i++) {
		ioManager::log("Initialising " + possiblePhases[i] + " phase...", 0, 1);
		x[i] = findXMLPhase(xc, possiblePhases[i]);		// find phase XML input
		if (x[i] == NULL) {											// display error message if phase not found
			ioManager::log("Could not find " + possiblePhases[i] + " phase in file", 0, 1); 
			if (i == POSSIBLEPHASES_SOLIDX || i == POSSIBLEPHASES_SURFIDX) {
				throw("error!");
			}
		}
		else {
			xTP = newPhase(*x[i]);							// create thermo phase object from XML, xTP points to it
			if (i == POSSIBLEPHASES_SOLIDX) {
				solPhaseIdx = j;
			}
			if (i == POSSIBLEPHASES_SURFIDX) {
				surfPhaseIdx = j;
			}
			phaseList.push_back(xTP);
			j++;
			
		}
	}
	nrPhases = j;


	// SET UP KINETICS
	// Create kinetics manager for heterogeneous reaction mechanisms.
	iKin = new InterfaceKinetics();		// 2D interphase at Cu surface
	importKinetics(*x[POSSIBLEPHASES_SURFIDX], phaseList, iKin);					// import reaction mechanism for Cu surface
	// get and display number of reactions at the interface
	nrReac = iKin->nReactions();
	ioManager::log("Number of reactions at surface = " + nrReac, 0, 1);
	// put InterfaceKinetics pointer in vector for chemistry set up
	vector<InterfaceKinetics*> vecKinPtrs{ iKin };

	// FIND ELECTROCHEMICAL REACTIONS
	//TODO we don't really need this anymore?
	electron_idx = iKin->kineticsSpeciesIndex("e-");
	int nrReac_ec = 0;
	electron_transfer = vector<int>(nrReac);
	for (int i = 0; i < nrReac; i++) {
		if (iKin->reactantStoichCoeff(electron_idx,i) != -1) {
			electron_transfer[i] += iKin->reactantStoichCoeff(electron_idx,i);
		}
		if (iKin->productStoichCoeff(electron_idx,i) != -1) {
			electron_transfer[i] -= iKin->productStoichCoeff(electron_idx,i);
		}
		if (electron_transfer[i] != 0) {
			nrReac_ec += 1;
		}
	}
	ioManager::log("Number of electrochemical reactions = " + to_string(nrReac_ec), 0, 1); 

	int k = 0;
	for (int i = 0; i < nrPhases; i++) {
		int tempNoSpec = phaseList[i]->nSpecies();
		for (int j = 0; j < tempNoSpec; j++) {
			//vector<const XML_Node*> tempXml = phaseList[i]->speciesData();
			allSpecies[k] = phaseList[i]->species(j);

			k++;
		}
	}

	// FIND SURFACE SPECIES
	surfacePhase = (Cantera::SurfPhase*) phaseList[surfPhaseIdx];
	nrSurfSpec = surfacePhase->nSpecies();
	string tempName;
	for (int i = 0; i < nrSurfSpec; i++) {
		tempName = surfacePhase->speciesName(i);
		for (int k = 0; k < nrSpecies; k++) {
			if (allSpecies[k]->name == tempName) {
				surfSpec.push_back(k);
				break;
			}
		}
	}
	ioManager::log("Number of surface species = " + to_string(nrSurfSpec), 0, 1); 

	// SAVE INITIAL CONDITIONS
	double *c0{ new double[nrSpecies] {} };
	j = 0;
	for (int i = 0; i < nrPhases; i++) {
		phaseList[i]->getConcentrations(&c0[j]);
		j += phaseList[i]->nSpecies();
	}
	for (int j = 0; j < nrSpecies; j++) {
		c_init.push_back(c0[j]);
	}

	// initialize variables
	nrProducts = ioManager::getNoOfProducts();
	setZeros();

	// check if individual output file for this reactor shall be created
	int tempOO = ioManager::getFileOutputOpts();
	individualOutput = (tempOO == ioManager::OUTPUT_ALL || tempOO == ioManager::OUTPUT_TRANSIENT);

	// set steady state criteria
	ssCrit = stof(ioManager::getSteadyStateCriterion());
	ssTimeMax = 36000;

	// set averaging values
	t_avg = stof(ioManager::getFinalMeanDuration());
	n_avg = ioManager::getFinalMeanSteps();

	// set solver parameters
	relTol = stof(ioManager::getSolverRelativeTolerance());
	absTol = stof(ioManager::getSolverAbsoluteTolerance());
}

void Cat_reactor::copyResults(Cat_reactor* reactor2copy) {
	// set scalar values
	didt_rel = reactor2copy->relChangeI();
	FE_sum = reactor2copy->sumFE();
	lastChangeTime = reactor2copy->ssTime();

	nrSpecies = reactor2copy->speciesCount();
	nrReac = reactor2copy->reactionCount();
	nrProducts = reactor2copy->productCount();
	nrSurfSpec = reactor2copy->surfaceSpeciesCount();

	setZeros();

	/*allSpecies = reactor2copy->allSpecies;
	phaseList = reactor2copy->phaseList;
	iKin = reactor2copy->iKin;
	vecKinPtrs = reactor2copy->vecKinPtrs;
	surfacePhase = reactor2copy->surfacePhase;
	timeStamp = reactor2copy->timeStamp;
	c_init = reactor2copy->c_init;
	surfPhaseIdx = reactor2copy->surfPhaseIdx;
	solPhaseIdx = reactor2copy->solPhaseIdx;
	electron_idx = reactor2copy->electron_idx;
	electron_transfer = reactor2copy->electron_transfer;
	surfSpec = reactor2copy->surfSpec;
	individualOutput = reactor2copy->individualOutput;
	maxStep = reactor2copy->maxStep;
	minStep = reactor2copy->minStep;
	initStep = reactor2copy->initStep;
	ssCrit = reactor2copy->ssCrit;
	ssFlag = reactor2copy->ssFlag;
	lastChangeTime = reactor2copy->lastChangeTime;
	ssTimeMax = reactor2copy->ssTimeMax;
	dt = reactor2copy->dt;*/
	t = reactor2copy->t;
	pot = reactor2copy->pot;

	// set species output variables
	for (int i = 0; i < nrSpecies; i++) {
		conc[i] = reactor2copy->speciesConcentration(i);
		act[i] = reactor2copy->speciesActivity(i);
		net[i] = reactor2copy->speciesNetProduction(i);
	}
	
	// set reaction output variables
	for (int i = 0; i < nrReac; i++) {
		fwdRC[i] = reactor2copy->forwardReactionCoefficient(i);
		revRC[i] = reactor2copy->reverseReactionCoefficient(i);
		fwdRR[i] = reactor2copy->forwardReactionRate(i);
		revRR[i] = reactor2copy->reverseReactionRate(i);
		netRR[i] = reactor2copy->netReactionRate(i);
	}
	
	// set product output variables
	for (int i = 0; i < nrProducts; i++) {
		i_k[i] = reactor2copy->productCurrDens(i);
		faradEff[i] = reactor2copy->productFE(i);
		dFEdt[i] = reactor2copy->changeFE(i);
	}
	
	// set surface species output variables
	for (int i = 0; i < nrSurfSpec; i++) {
		cov[i] = reactor2copy->speciesCoverage(i);
		intermFE[i] = reactor2copy->intermediateFE(i);
	}
	
}

void Cat_reactor::averageResults(vector<Cat_reactor*> reactors) {
	
	int n_avg = ioManager::getFinalMeanSteps();
	
	// vectors
	setZeros();
	for (int i = 0; i < nrSpecies; i++) {
		conc[i] = 0;
		act[i] = 0;
		net[i] = 0;
		for (int j = 1; j < reactors.size(); j++) {
			conc[i] += reactors[j]->speciesConcentration(i);
			act[i] += reactors[j]->speciesActivity(i);
			net[i] += reactors[j]->speciesNetProduction(i);
		}
		conc[i] = conc[i] / n_avg;
		act[i] = act[i] / n_avg;
		net[i] = net[i] / n_avg;
	}

	for (int i = 0; i < nrReac; i++) {
		fwdRC[i] = 0;
		revRC[i] = 0;
		fwdRR[i] = 0;
		revRR[i] = 0;
		netRR[i] = 0;
		for (int j = 1; j < reactors.size(); j++) {
			fwdRC[i] += reactors[j]->forwardReactionCoefficient(i);
			revRC[i] += reactors[j]->reverseReactionCoefficient(i);
			fwdRR[i] += reactors[j]->forwardReactionRate(i);
			revRR[i] += reactors[j]->reverseReactionRate(i);
			netRR[i] += reactors[j]->netReactionRate(i);
		}
		fwdRC[i] = fwdRC[i] / n_avg;
		revRC[i] = revRC[i] / n_avg;
		fwdRR[i] = fwdRR[i] / n_avg;
		revRR[i] = revRR[i] / n_avg;
		netRR[i] = netRR[i] / n_avg;
	}

	// steady-state scalars
	dt = reactors.back()->currentTime() - reactors[0]->currentTime();
	double* tempI = new double[nrProducts];

	for (int i = 0; i < nrProducts; i++) {
		i_k[i] = reactors.back()->productCurrDens(i);
		lastI[i] = reactors[0]->productCurrDens(i);
		tempI[i] = 0;
		for (int j = 1; j < reactors.size(); j++) {
			tempI[i] += reactors[j]->productCurrDens(i);
		}
		tempI[i] = tempI[i] / n_avg;
		faradEff[i] = tempI[i] / tempI[0];
	}
	checkSteadyState(false);


	// rest of vectors
	for (int i = 0; i < nrProducts; i++) {
		dFEdt[i] = 0;
		for (int j = 1; j < reactors.size(); j++) {
			dFEdt[i] += reactors[j]->changeFE(i);
		}
		i_k[i] = tempI[i];
		dFEdt[i] = dFEdt[i] / n_avg;
	}

	for (int i = 0; i < nrSurfSpec; i++) {
		intermFE[i] = 0;
		cov[i] = 0;
		for (int j = 1; j < reactors.size(); j++) {
			intermFE[i] += reactors[j]->intermediateFE(i);	// should be updated accordingly (product FEs are calculated from averaged currents)
			cov[i] += reactors[j]->speciesCoverage(i);
		}
		intermFE[i] = intermFE[i] / n_avg;
		cov[i] = cov[i] / n_avg;
	}
}