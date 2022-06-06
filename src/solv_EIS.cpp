#include <iostream>						// Provides standard input/output stream objects (i.e. cout)
#include <fstream>						// Provides stream class to  read and write files (i.e. ofstream)
#include <sstream>

#include "Cat_reactor.h"				// Header file that contains this 
/* -----------------------------------------
-------------DEPRECATED CLASS!!!------------
--------------------------------------------*/

//TODO integrate this to solv_trans
void Cat_reactor::solv_EIS(double phi_amp, double phi_freq, int nrSteps) {
	// SET UP OUTPUT
	// create output file
	stringstream temp;
	temp.precision(1);
	temp << phi_freq;
	string ExpFileName = "EIS_results_"+ temp.str() + "Hz_" + timeStamp + ".csv";
	ofstream f2(ExpFileName);	// create CSV
	f2.precision(16);			// control the output precision 
	f2 << "time [s]" << ", " << "Potential difference [V]" << ", " << "Current density [A/m2]" << ", ";
	// initialize species output variables
	double *c{ new double[nrSpecies] {} };
	// write column header for species output variables
	for (int i = 0; i < nrSpecies; i++) {
		f2 << "[" << allSpecies[i].get()->name << "]" << ", ";
	}
	// initialize reaction output variables
	double *k_f{ new double[nrReac] {} };
	double *k_r{ new double[nrReac] {} };
	double *r_f{ new double[nrReac] {} };
	double *r_r{ new double[nrReac] {} };
	double i_e;
	// write column header for reaction output variables
	for (int i = 0; i < nrReac; i++) {
		f2 << "k_f(" << to_string(i) << ")" << ", ";
		f2 << "k_r(" << to_string(i) << ")" << ", ";
		f2 << "r_f(" << to_string(i) << ")" << ", ";
		f2 << "r_r(" << to_string(i) << ")" << ", ";
	}
	f2 << endl;

	double t_end = 5 / phi_freq;
	double delta_t = t_end / nrSteps;
	double t, phi;
	double phi_omega = 2 * Pi * phi_freq;

	for (int t_step = 0; t_step <= nrSteps; t_step++) {
		t = t_step * delta_t;
		phi = phi_amp * sin(phi_omega*t);

		phaseList[1]->setElectricPotential(phi);
		phaseList[3]->setElectricPotential(phi);

		iKin->updateROP();

		iKin->getFwdRateConstants(k_f);
		iKin->getRevRateConstants(k_r);
		iKin->getFwdRatesOfProgress(r_f);
		iKin->getRevRatesOfProgress(r_r);
		i_e = (r_f[0] - r_r[0])*Faraday;

		int j = 0;
		for (int i = 0; i < nrPhases; i++) {
			phaseList[i]->getConcentrations(&c[j]);
			j += phaseList[i]->nSpecies();
		}

		// CREATE OUTPUT
		f2 << t << ", " << phi << ", " << i_e << ", ";
		for (int i = 0; i < nrSpecies; i++) {
			f2 << c[i] << ", ";
		}
		for (int i = 0; i < nrReac; i++) {
			f2 << k_f[i] << ", ";
			f2 << k_r[i] << ", ";
			f2 << r_f[i] << ", ";
			f2 << r_r[i] << ", ";
		}
		f2 << endl;

		iKin->advanceCoverages(delta_t);
	}


	f2.close();

}