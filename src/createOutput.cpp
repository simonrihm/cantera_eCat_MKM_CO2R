#include "ioManager.h"
ofstream ioManager::logStream;

ofstream ioManager::createOutput(string name, bool toBeWritten) {
	if (toBeWritten) { // create output file
	
		if (name.compare("") == 0) {
			name = ioManager::defaultOutputName;
		}

		string expFileName = getOutputPath() + name + ".csv";
		ioManager::log("Write results to " + expFileName, 0, 1); 
		ofstream f(expFileName);	// create CSV
		f.precision(16);			// control the output precision 
		return f;
	}
	else { // if no file should be written, false can be passed to this method and closed ofstream is created
		ofstream f1;
		return f1;
	}
}

void ioManager::createLog(string path) {
	logStream.open(path, ios_base::out);
	logStream << "===== INPUT FILE COPY =====" << endl;
	std::ifstream inFile(inputPath);
	logStream << inFile.rdbuf();
	inFile.close();
	logStream << endl << endl << "===== SIMULATION LOG =====" << endl;
}

void ioManager::log(string stringToWrite, int nrEndlBefore, int nrEndlAfter) {
	if (logOutputOpts != LOG_SILENT) {
		for (int i = 0; i < nrEndlBefore; i++) {
			cout << endl;
			if (logStream.is_open()) {
				logStream << endl;
			}
		}

		cout << stringToWrite;
		if (logStream.is_open()) {
			logStream << stringToWrite;
		}

		for (int i = 0; i < nrEndlAfter; i++) {
			cout << endl;
			if (logStream.is_open()) {
				logStream << endl;
			}
		}
	}
	
}