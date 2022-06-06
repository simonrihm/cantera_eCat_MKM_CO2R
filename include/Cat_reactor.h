#pragma once	//this inhibits multiple inclusions of the same header, leading to "class type redifinition" and "undeclared identifier" errors

#include <string>
#include <vector>
#include "thermo.h"
#include "kinetics.h"
#include "cantera/thermo/SurfPhase.h"
#include "cantera/kinetics/InterfaceKinetics.h"
#include "varManager.h"
#include "anaManager.h"

//using namespace std;					// standard namespace is needed e.g. for string 
using namespace Cantera;				// Cantera namespace is needed to access Cantera classes

extern bool timeoutFlag;

class Cat_reactor
{
	public:
		//TODO Rule of 3
		Cat_reactor() {};
		Cat_reactor(string MechFileName);
		//Cat_reactor(const Cat_reactor& copiedReactor);
		//~Cat_reactor();

		void solv_EIS(double phi_amp, double phi_freq, int nrSteps);
		bool controlSteps_SS(double maxRates, double maxDiff);
		bool controlSteps_t0(double t0, varManager* phiFun);

		void setZeros();
		void reset();
		void setPot(double phi);
		void evaluateStep();
		int timestep(double delta_t);
		void setStepping(double init, double max, double min);
		bool checkSteadyState(bool feedback);
		
		void averageResults(vector<Cat_reactor*>reactors);
		void copyResults(Cat_reactor* reactor2Copy);
		void terminateCopy();

		double currentTime() const { return t; };
		double getPotential() const { return pot; };

		double speciesConcentration(int i) const { return conc[i]; };
		double speciesActivity(int i) const { return act[i]; };
		double speciesNetProduction(int i) const { return net[i]; };
		double speciesCoverage(int i) const { return cov[i]; };
		double productCurrDens(int i) const { return i_k[i]; };
		double productFE(int i) const { return faradEff[i]; };

		double relChangeI() const { return didt_rel; };
		double sumFE() const { return FE_sum; };
		double changeFE(int i) const { return dFEdt[i]; };
		double intermediateFE(int i) const { return intermFE[i]; };
		double ssTime() const { return lastChangeTime; };

		double forwardReactionCoefficient(int i) const { return fwdRC[i]; };
		double reverseReactionCoefficient(int i) const { return revRC[i]; };
		double forwardReactionRate(int i) const { return fwdRR[i]; };
		double reverseReactionRate(int i) const { return revRR[i]; };
		double netReactionRate(int i) const { return netRR[i]; };

		int speciesCount() const { return nrSpecies; };
		int phaseCount() const { return nrPhases; };
		int reactionCount() const { return nrReac; };
		int surfaceSpeciesCount() const { return nrSurfSpec; };
		int productCount() const { return nrProducts; };

		string speciesName(int i) const { return allSpecies[i].get()->name; };
		string surfaceSpeciesName(int i) const { return allSpecies[surfSpec[i]].get()->name; };
		string reactionName(int i) const { return iKin->reaction(i)->id; };

		int getSpeciesIndex(string species);
		
	private: 
		vector<shared_ptr<Species>> allSpecies;
		vector<ThermoPhase*> phaseList;				// put all thermo phase objects in a list to reach phases by index number
		InterfaceKinetics* iKin;
		vector<InterfaceKinetics*> vecKinPtrs;
		int nrSpecies, nrReac, nrSurfSpec, nrProducts;
		SurfPhase* surfacePhase;
		string timeStamp;
		vector<double> c_init;
		int nrPhases = 3;
		int surfPhaseIdx;
		int solPhaseIdx;
		//string *phaseNames;
		//const string phaseNames[3] = { "solid","liquid","surface" };
		/*const static int nrPhases = 4;
		const int surfPhaseIdx = 3;
		const string phaseNames[4] = { "gas","solid","liquid","surface" };*/
		const string possiblePhases[4] = { "gas","solid","liquid","surface" };
		int electron_idx;
		vector<int> electron_transfer;
		vector<int> surfSpec;
		void getSpecCurrDens(double* i_k);
		bool individualOutput;

		const static int POSSIBLEPHASES_NUMBER = 4;
		const static int POSSIBLEPHASES_SURFIDX = 3;
		const static int POSSIBLEPHASES_SOLIDX = 1;

		double maxStep;
		double minStep;
		double initStep;

		double relTol;
		double absTol;

		double ssCrit;
		bool ssFlag;
		double lastChangeTime;
		double ssTimeMax;

		double t_avg;
		double n_avg;
		
		double* i_k;
		double* lastI;
		double* faradEff;

		double FE_sum;
		double didt_rel;
		double* dFEdt;
		double* intermFE;

		double dt;
		double t;
		double pot;

		double* conc;
		double* net;
		double* cov;
		double* act;

		double* fwdRC;
		double* revRC;
		double* fwdRR;
		double* revRR;
		double* netRR;
};