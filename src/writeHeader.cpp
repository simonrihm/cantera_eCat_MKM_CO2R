#include "ioManager.h"

void ioManager::writeHeader(Cat_reactor* reactor, ofstream& f, string additionalDataHeader) {
	// CREATE OUTPUT HEADER

	// if closed ofstream is passed, nothing is to be written (see "createOutput")
	if (!f.is_open()) {
		return;
	}

	// write header for main variables, time and potential
	f << "t [s]" << "," << "U [V]" << ",";

	// write column header for specific current densities and Faradaic Efficiencies
	int currOO = ioManager::getCurrOutputOpts();
	if (currOO == ioManager::CURRENT_ALL) {
		f << "i(" << productNames[0] << ")" << ",";
		for (int i = 1; i < productNames.size(); i++) {
			f << "i(" << productNames[i] << ")" << ",";
			f << "FE(" << productNames[i] << ")" << ",";
		}
	}
	if (currOO == ioManager::CURRENT_SPEC) {
		for (int i = 0; i < productNames.size(); i++) {
			f << "i(" << productNames[i] << ")" << ",";
		}
	}
	if (currOO == ioManager::CURRENT_FE) {
		f << "i(" << productNames[0] << ")" << ",";
		for (int i = 1; i < productNames.size(); i++) {
			f << "FE(" << productNames[i] << ")" << ",";
		}
	}

	// write column header for general species output variables
	int specOO = ioManager::getSpecOutputOpts();
	for (int i = 0; i < reactor->speciesCount(); i++) {
		if (specOO != ioManager::SPECIES_NONE) {
			f << "q(" << reactor->speciesName(i) << ")" << ",";
			if (specOO != ioManager::SPECIES_RATES) {
				f << "[" << reactor->speciesName(i) << "]" << ",";
				f << "a(" << reactor->speciesName(i) << ")" << ",";
			}
		}
	}

	// write column header for intermediate coverages
	if (ioManager::getCovOutputOpts() == ioManager::COVERAGE_ON) {
		for (int i = 0; i < reactor->surfaceSpeciesCount(); i++) {
			f << "x(" << reactor->surfaceSpeciesName(i) << ")" << ",";
		}
	}
	

	// write column header for reaction output variables
	int rxnOO = ioManager::getRxnOutputOpts();
	for (int i = 0; i < reactor->reactionCount(); i++) {
		if (rxnOO != ioManager::REACTION_NONE) {
			f << "r_net(" << reactor->reactionName(i) << ")" << ",";
			if (rxnOO != ioManager::REACTION_NET) {
				f << "r_f(" << reactor->reactionName(i) << ")" << ",";
				f << "r_r(" << reactor->reactionName(i) << ")" << ",";
				if (rxnOO != ioManager::REACTION_RATES) {
					f << "k_f(" << reactor->reactionName(i) << ")" << ",";
					f << "k_r(" << reactor->reactionName(i) << ")" << ",";
				}
			}
		}
	}

	// write column header for change rates
	int ddtOO = ioManager::getDdtOutputOpts();
	if (ddtOO != ioManager::DDT_NONE) {
		f << "d(i_rel)/dt" << ",";
		f << "sum(FE)" << ",";
		f << "t_SS" << ",";
		if (ddtOO != ioManager::DDT_SS) {
			for (int i = 1; i < productNames.size(); i++) {
				f << "d(FE(" << productNames[i] << "))/dt" << ",";
			}
			if (ddtOO != ioManager::DDT_PROD) {
				for (int i = 0; i < reactor->surfaceSpeciesCount(); i++) {
					f << "FE(" << reactor->surfaceSpeciesName(i) << ")" << ",";
				}
			}
		}
	}

	// write additional header and end line
	f << additionalDataHeader << endl;
}