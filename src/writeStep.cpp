#include "ioManager.h"
#include "Cat_reactor.h"	

void ioManager::writeStep(Cat_reactor* reactor, ofstream& f, string additionalDataItem) {
	// CREATE OUTPUT

	// if closed ofstream is passed, nothing is to be written (see "createOutput")
	if (!f.is_open()) {
		return;
	}

	// write main variables, time and potential
	f << reactor->currentTime() << "," << reactor->getPotential() << ",";

	// write specific current densities and Faradaic Efficiencies
	int currOO = ioManager::getCurrOutputOpts();
	if (currOO == ioManager::CURRENT_ALL) {
		f << reactor->productCurrDens(0) << ",";
		for (int i = 1; i < productNames.size(); i++) {
			f << reactor->productCurrDens(i) << ",";
			f << reactor->productFE(i) << ",";
		}
	}
	if (currOO == ioManager::CURRENT_SPEC) {
		for (int i = 0; i < productNames.size(); i++) {
			f << reactor->productCurrDens(i) << ",";
		}
	}
	if (currOO == ioManager::CURRENT_FE) {
		f << reactor->productCurrDens(0) << ",";
		for (int i = 1; i < productNames.size(); i++) {
			f << reactor->productFE(i) << ",";
		}
	}

	// write column header for general species output variables
	int specOO = ioManager::getSpecOutputOpts();

	for (int i = 0; i < reactor->speciesCount(); i++) {
		if (specOO != ioManager::SPECIES_NONE) {
			f << reactor->speciesNetProduction(i) << ",";
			if (specOO != ioManager::SPECIES_RATES) {
				f << reactor->speciesConcentration(i) << ",";
				f << reactor->speciesActivity(i) << ",";
			}
		}
	}

	// write intermediate coverages
	if (ioManager::getCovOutputOpts() == ioManager::COVERAGE_ON) {
		for (int i = 0; i < reactor->surfaceSpeciesCount(); i++) {
			f << reactor->speciesCoverage(i) << ",";
		}
	}

	// write column header for reaction output variables
	int rxnOO = ioManager::getRxnOutputOpts();
	for (int i = 0; i < reactor->reactionCount(); i++) {
		if (rxnOO != ioManager::REACTION_NONE) {
			f << reactor->netReactionRate(i) << ",";
			if (rxnOO != ioManager::REACTION_NET) {
				f << reactor->forwardReactionRate(i) << ",";
				f << reactor->reverseReactionRate(i) << ",";
				if (rxnOO != ioManager::REACTION_RATES) {
					f << reactor->forwardReactionCoefficient(i) << ",";
					f << reactor->reverseReactionCoefficient(i) << ",";
				}
			}
		}
	}

	// write column for change rates
	int ddtOO = ioManager::getDdtOutputOpts();
	if (ddtOO != ioManager::DDT_NONE) {
		f << reactor->relChangeI() << ",";
		f << reactor->sumFE() << ",";
		f << reactor->ssTime() << ",";
		if (ddtOO != ioManager::DDT_SS) {
			for (int i = 1; i < productNames.size(); i++) {
				f << reactor->changeFE(i) << ",";
			}
			if (ddtOO != ioManager::DDT_PROD) {
				for (int i = 0; i < reactor->surfaceSpeciesCount(); i++) {
					f << reactor->intermediateFE(i) << ",";
				}
			}
		}
	}
	
	// write additional item and end line
	f << additionalDataItem << endl;
}