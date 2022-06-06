#include "Cat_reactor.h"				// Header file that contains this 
#include "ioManager.h"

void Cat_reactor::reset() {
	double *c0{ new double[nrSpecies] {} };
	int k = 0;
	for (int i = 0; i < nrPhases; i++) {
		for (int j = 0; j < phaseList[i]->nSpecies(); j++) {
			c0[k] = c_init[k];
			k++;
		}
		try {
			phaseList[i]->setConcentrations(&c_init[k - phaseList[i]->nSpecies()]);
		}
		catch (Cantera::CanteraError& err)
		{
			//TODO we only need to reset the surface phase, the rest is constant anyways
			ioManager::log("Could not reset concentrations of phase " + phaseList[i]->name(), 0, 1);
		}
	}
	timeStamp = ioManager::createTimeStamp();
	timeoutFlag = false;
}

void Cat_reactor::setPot(double phi) {
	pot = phi;
	phaseList[solPhaseIdx]->setElectricPotential(pot);
	phaseList[surfPhaseIdx]->setElectricPotential(pot);
}

void Cat_reactor::setZeros() {
	conc = new double[nrSpecies];
	act = new double[nrSpecies];
	net = new double[nrSpecies];

	fwdRC = new double[nrReac];
	revRC = new double[nrReac];
	fwdRR = new double[nrReac];
	revRR = new double[nrReac];
	netRR = new double[nrReac];

	i_k = new double[nrProducts];
	faradEff = new double[nrProducts];
	dFEdt = new double[nrProducts];
	lastI = new double[nrProducts];

	cov = new double[nrSurfSpec];
	intermFE = new double[nrSurfSpec];
}

/*Cat_reactor::~Cat_reactor() {
	allSpecies.clear();
	for (int i = 0; i < phaseList.size(); i++) {
		delete phaseList[i];
	}
	phaseList.clear();
	delete iKin;
	for (int i = 0; i < vecKinPtrs.size(); i++) {
		delete vecKinPtrs[i];
	}
	vecKinPtrs.clear();
	delete surfacePhase;
	c_init.clear();
	electron_transfer.clear();
	surfSpec.clear();

	delete i_k;
	delete lastI;
	delete faradEff;

	delete conc;
	delete net;
	delete cov;
	delete act;

	delete fwdRC;
	delete revRC;
	delete fwdRR;
	delete revRR;
	delete netRR;
}*/

void Cat_reactor::terminateCopy() {
	delete[] i_k;
	delete[] lastI;
	delete[] faradEff;
	delete[] dFEdt;
	delete[] intermFE;

	delete[] conc;
	delete[] net;
	delete[] cov;
	delete[] act;

	delete[] fwdRC;
	delete[] revRC;
	delete[] fwdRR;
	delete[] revRR;
	delete[] netRR;
}