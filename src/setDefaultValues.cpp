#include "ioManager.h"

string ioManager::xml;
string ioManager::csv;
int ioManager::noStep;
string ioManager::endTime;
string ioManager::inputPath;
string ioManager::outputPath;
bool ioManager::requireInteraction;
string ioManager::var;
string ioManager::phi;
string ioManager::maxTimeStep;
string ioManager::minTimeStep;
string ioManager::initTimeStep;
string ioManager::steadyStateCriterion;
string ioManager::finalMeanDuration;
string ioManager::solverAbsoluteTolerance;
string ioManager::solverRelativeTolerance;
int ioManager::finalMeanSteps;
vector<string> ioManager::productNames;
int ioManager::specOutputOpts;
int ioManager::rxnOutputOpts;
int ioManager::fileOutputOpts;
int ioManager::covOutputOpts;
int ioManager::currOutputOpts;
int ioManager::nameOutputOpts;
int ioManager::sampleOutputOpts;
int ioManager::ddtOutputOpts;
int ioManager::logOutputOpts;
string ioManager::defaultOutputName;
int ioManager::csv_length;

//TODO consolidate ioManager classes in fewer files (e.g. this one to ioManager.cpp)

void ioManager::setDefaultValues() {
	xml = "InputFiles/mechanism.xml";
	csv = "noCSVbyDefault";
	csv_length = 0;
	inputPath = "InputFiles/input.txt";
	outputPath = "Results";
	noStep = 1;
	endTime = 1.0;
	maxTimeStep = "0.1";
	minTimeStep = "0.0";
	initTimeStep = "0.0";
	steadyStateCriterion = "1e-5";
	requireInteraction = true;
	var = "const(0)";
	phi = "const(-1)";
	defaultOutputName = "output";
	finalMeanDuration = "0.0";
	finalMeanSteps = 0;
	solverRelativeTolerance = "1e-10";
	solverAbsoluteTolerance = "1e-20";

	productNames.push_back("e-");	//first product is treated as reference for FE calculation
	productNames.push_back("H2");
	productNames.push_back("CO");
	productNames.push_back("C2H4");

	specOutputOpts = SPECIES_ALL;
	rxnOutputOpts = REACTION_ALL;
	covOutputOpts = COVERAGE_ON;
	currOutputOpts = CURRENT_ALL;
	fileOutputOpts = OUTPUT_ALL;
	nameOutputOpts = FILENAME_TIMESTAMP;
	sampleOutputOpts = SAMPLE_NONE;
	ddtOutputOpts = DDT_NONE;
	logOutputOpts = LOG_NONE;

	requireInteraction = true;
}