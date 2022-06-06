#pragma once

#include <string>
#include <vector>
#include "Cat_reactor.h"
#include <fstream>


class ioManager {
	public:
		static void readInputFile();
		static vector<string> split(string s, char delimiter);
		static void setDefaultValues();
		static string createTimeStamp();

		static string getXml() { return xml; };
		static void copyFile(string in, string out);
		static void setInputPath(string in) { inputPath = in; };
		static void setOutputPath(string out) { outputPath = out; };
		static string getOutputPath();
		static bool getReqInteract() { return requireInteraction; };
		static void setReqInteract(string ri) { if (ri.compare("false")==0) { requireInteraction = false; } };
		static void writeStep(Cat_reactor* reactor, ofstream& f, string additionalDataItem);
		static void writeHeader(Cat_reactor* reactor, ofstream& f, string additionalDataHeader);
		static ofstream createOutput(string timeStamp, bool toBeWritten);
		static void createLog(string path);
		static void log(string stringToWrite, int nrEndlBefore, int nrEndlAfter);

		static string getMaxTimeStep() { return maxTimeStep; };
		static string getMinTimeStep() { return minTimeStep; };
		static string getInitTimeStep() { return initTimeStep; };
		static string getSolverRelativeTolerance() { return solverRelativeTolerance; };
		static string getSolverAbsoluteTolerance() { return solverAbsoluteTolerance; };
		static string getEndTime() { return endTime; };
		static string getSteadyStateCriterion() { return steadyStateCriterion; };
		static string getVar() { return var; };
		static string getPhi() { return phi; };
		static string getFinalMeanDuration() { return finalMeanDuration; };
		static int getFinalMeanSteps() { return finalMeanSteps; };
		static int getNoStep() { return noStep; };
		static int getCsvLength() { return csv_length; };

		static string getProductName(int i) { return productNames[i]; };
		static int getNoOfProducts() { return productNames.size(); }

		static int getSpecOutputOpts() { return specOutputOpts; };
		static int getRxnOutputOpts() { return rxnOutputOpts; };
		static int getCovOutputOpts() { return covOutputOpts; };
		static int getCurrOutputOpts() { return currOutputOpts; };
		static int getFileOutputOpts() { return fileOutputOpts; };
		static int getNameOutputOpts() { return nameOutputOpts; };
		static int getSampleOutputOpts() { return sampleOutputOpts; };
		static int getDdtOutputOpts() { return ddtOutputOpts; };
		static bool getTxtLogOpts() { return logOutputOpts == LOG_TXT || logOutputOpts == LOG_ALL; };
		static bool getCopyMechanismOpts() { return logOutputOpts == LOG_INPUT || logOutputOpts == LOG_ALL; };

		static double getCsvVal(string var, int row);
		static double* getCsvCol(string var);
		static int checkCsvFile();
		static string exec(const char* cmd);
		static ofstream logStream;

		// overall output file options: fileOutputOpts
		static const int OUTPUT_ALL = 0;
		static const int OUTPUT_CUMULATED = 1;
		static const int OUTPUT_TRANSIENT = 2;
		static const int OUTPUT_NONE = -1; // only for testing, does not make sense for production...

		// reaction output options: rxnOutputOpts
		static const int REACTION_ALL = 0;
		static const int REACTION_RATES = 1;
		static const int REACTION_NET = 2;
		static const int REACTION_NONE = -1;

		// species output options: specOutputOpts
		static const int SPECIES_ALL = 0;
		static const int SPECIES_RATES = 1;
		static const int SPECIES_NONE = -1;

		// product output options: currOutputOpts
		static const int CURRENT_ALL = 0;
		static const int CURRENT_FE = 1;
		static const int CURRENT_SPEC = 2;
		static const int CURRENT_NONE = -1;

		// intermediate output options: covOutputOpts
		static const int COVERAGE_ON = 0;
		static const int COVERAGE_OFF = -1;

		// name of output file options: nameOutputOpts
		static const int FILENAME_TIMESTAMP = 0;
		static const int FILENAME_OUTPUT = 1;

		// sampling of output: sampleOutputOpts
		static const int SAMPLE_NONE = 0;
		static const int SAMPLING_T10 = 1;

		// change rate output options: ddtOutputOpts
		static const int DDT_NONE = 0;
		static const int DDT_SS = 1;
		static const int DDT_PROD = 2;
		static const int DDT_ALL = 3;

		// logging output options: logOutputOpts
		static const int LOG_SILENT = -1;
		static const int LOG_NONE = 0;
		static const int LOG_TXT = 1;
		static const int LOG_INPUT = 2;
		static const int LOG_ALL = 3;

	private:
		ioManager();
		static string inputPath;
		static string outputPath;
		static string defaultOutputName;
		static bool requireInteraction;

		static string xml;
		static string csv;
		static int csv_length;
		static int noStep;
		static string phi;
		static string var;
		static string endTime;
		static string maxTimeStep;
		static string minTimeStep;
		static string initTimeStep;
		static string solverRelativeTolerance;
		static string solverAbsoluteTolerance;
		static string steadyStateCriterion;
		static string finalMeanDuration;
		static int finalMeanSteps;

		static vector<string> productNames;

		static int specOutputOpts;
		static int rxnOutputOpts;
		static int covOutputOpts;
		static int currOutputOpts;
		static int fileOutputOpts;
		static int nameOutputOpts;
		static int sampleOutputOpts;
		static int ddtOutputOpts;
		static int logOutputOpts;

		static bool stringToBool(string s);

};