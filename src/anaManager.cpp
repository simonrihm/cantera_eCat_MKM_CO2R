#include "anaManager.h"

using namespace std;

double anaManager::getCurrDens(string species, double netProd) {
	double factor;

	if (species.compare("e-") == 0) {
		factor = -1;
	}
	else {
		factor = redState(species);
	}

	return factor * Faraday * netProd;
}

int anaManager::redState(string molecule) {
	int nr_atoms[] = { 0,0,0,0,0 };
	analyseMolecule(nr_atoms, molecule);
	int totalOxNo = nr_atoms[H] - 2 * nr_atoms[O];
	return totalOxNo + nr_atoms[C] * 4;
}

void anaManager::analyseMolecule(int* nr_atoms, string molecule) {
	int nr_atoms_temp[] = { 0,0,0,0,0 };

	int n = molecule.length();
	bool bracket_flag = false;
	int lastAtom;
	int number;

	for (int i = 0; i < n; i++) {
		switch(molecule[i]) {
			case 'C':
				if (bracket_flag) {
					nr_atoms_temp[C] ++;
				}
				nr_atoms[C] ++;
				lastAtom = C;
				break;
			case 'H':
				if (bracket_flag) {
					nr_atoms_temp[H] ++;
				}
				nr_atoms[H] ++;
				lastAtom = H;
				break;
			case 'O':
				if (bracket_flag) {
					nr_atoms_temp[O] ++;
				}
				nr_atoms[O] ++;
				lastAtom = O;
				break;
			case '*':
				if (bracket_flag) {
					nr_atoms_temp[Cu] = nr_atoms_temp[Cu] ++;
				}
				nr_atoms[Cu] ++;
				lastAtom = Cu;
				break;
			case '(':
				bracket_flag = true;
				break;
			case ')':
				bracket_flag = false;
				lastAtom = -1;
				break;
			case '+':
				nr_atoms[charge] ++;
				break;
			case '-':
				nr_atoms[charge] --;
				break;
			default:
				number = (int)(molecule[i] - '0');
				if (lastAtom == -1) {
					nr_atoms[C] = nr_atoms[C] + nr_atoms_temp[C] * (number - 1);
					nr_atoms[H] = nr_atoms[H] + nr_atoms_temp[H] * (number - 1);
					nr_atoms[O] = nr_atoms[O] + nr_atoms_temp[O] * (number - 1);
					nr_atoms[Cu] = nr_atoms[Cu] + nr_atoms_temp[Cu] * (number - 1);
					int nr_atoms_temp[] = { 0,0,0,0,0 };
				}
				else {
					if (bracket_flag) {
						nr_atoms_temp[lastAtom] = nr_atoms_temp[lastAtom] + (number - 1);
					}
					else {
						nr_atoms[lastAtom] = nr_atoms[lastAtom] + (number - 1);
					}
				}
		}	
	}
}