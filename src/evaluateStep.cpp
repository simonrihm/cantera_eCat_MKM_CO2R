#include "Cat_reactor.h"				// Header file that contains this 
#include "anaManager.h"
#include "ioManager.h"

void Cat_reactor::evaluateStep() {
	// calculate current system state variables after taking timestep
	ioManager::log("Solving transient problem for phi=" + to_string(pot) + " V at t = " + to_string(t) + " ...", 0, 1);
	iKin->updateROP();

	//TODO get rid of redundancy in input file checking between here and "readInputFile" (where errors are thrown)
	int rxnOO = ioManager::getRxnOutputOpts();
	if (rxnOO != ioManager::REACTION_NONE) {
		iKin->getNetRatesOfProgress(netRR);
		if (rxnOO != ioManager::REACTION_NET) {
			iKin->getFwdRatesOfProgress(fwdRR);
			iKin->getRevRatesOfProgress(revRR);
			if (rxnOO != ioManager::REACTION_RATES) {
				iKin->getFwdRateConstants(fwdRC);
				iKin->getRevRateConstants(revRC);
				if (rxnOO != ioManager::REACTION_ALL) {
					throw("error!");
				}
			}
		}
	}
	
	int specOO = ioManager::getSpecOutputOpts();
	if (specOO != ioManager::SPECIES_NONE) {
		iKin->getNetProductionRates(&net[0]);
		if (specOO != ioManager::SPECIES_RATES) {
			// get all concentrations and coverages
			int j = 0;
			for (int i = 0; i < nrPhases; i++) {
				phaseList[i]->getConcentrations(&conc[j]);
				phaseList[i]->getActivities(&act[j]);
				j += phaseList[i]->nSpecies();
			}
			if (specOO != ioManager::SPECIES_ALL) {
				throw("error!");
			}
		}
	}
	
	if (ioManager::getCovOutputOpts() == ioManager::COVERAGE_ON) {
		surfacePhase->getCoverages(&cov[0]);
	}

	int currOO = ioManager::getCurrOutputOpts();
	int ddtOO = ioManager::getDdtOutputOpts();
	if (currOO != ioManager::CURRENT_NONE) {
		if (ddtOO != ioManager::DDT_NONE) {
			for (int i = 0; i < nrProducts; i++) {
				lastI[i] = i_k[i];
			}
		}
		getSpecCurrDens(i_k);
		if (currOO == ioManager::CURRENT_FE || currOO == ioManager::CURRENT_ALL) {
			for (int i = 0; i < nrProducts; i++) {
				faradEff[i] = i_k[i] / i_k[0];
			}
		}
		else {
			if (currOO != ioManager::CURRENT_SPEC) {
				throw("error");
			}
		}		
	}
	
}

int Cat_reactor::getSpeciesIndex(string species) {
	//find index of allSpecies with allSpecies[i]->name == product
	auto it = std::find_if(allSpecies.begin(), allSpecies.end(), [&species](shared_ptr<Species> obj) {return obj->name == species; });
	int index = 0;
	if (it != allSpecies.end())
	{
		// found element. it is an iterator to the first matching element.
		// if you really need the index, you can also get it:
		index = std::distance(allSpecies.begin(), it);
	}
	return index;
}

void Cat_reactor::getSpecCurrDens(double* i_k) {
	int j;
	string productName;

	for (int i = 0; i < nrProducts; i++) {
		productName = ioManager::getProductName(i);
		j = getSpeciesIndex(productName);
		if (ioManager::getSpecOutputOpts() == ioManager::SPECIES_NONE) {
			iKin->getNetProductionRates(&net[0]);
		}
		i_k[i] = anaManager::getCurrDens(productName, net[j]);
	}
}

bool Cat_reactor::checkSteadyState(bool feedback) {
	int ddtOO = ioManager::getDdtOutputOpts();
	bool check = false;
	//TODO didt_rel seems to yield the wrong results, at least for first few timesteps.
	// This seems to be related to the order of evaluateStep and checkSteadyState in controlSteps funciton
	if (ddtOO != ioManager::DDT_NONE) {
		double didt_abs = (i_k[0] - lastI[0]) / dt;
		didt_rel = didt_abs / lastI[0];
		FE_sum = 0;
		for (int i = 1; i < nrProducts; i++) {
			FE_sum += faradEff[i];
		}

		check = (abs(FE_sum - 1) < ssCrit) && (abs(didt_rel) < ssCrit);

		if (ddtOO != ioManager::DDT_SS) {
			dFEdt[0] = 0.0;
			for (int i = 1; i < nrProducts; i++) {
				dFEdt[i] = (faradEff[i] - lastI[i] / lastI[0]) / dt;
			}

			if (ddtOO != ioManager::DDT_PROD) {
				string intermName;
				int iGlob;
				for (int i = 0; i < nrSurfSpec; i++) {
					intermName = surfaceSpeciesName(i);
					iGlob = getSpeciesIndex(intermName);
					intermFE[i] = anaManager::getCurrDens(intermName, net[iGlob]);
					intermFE[i] /= i_k[0];
				}
			}

		}
	}
	
	return check;
}