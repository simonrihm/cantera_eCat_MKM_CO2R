#include <iostream>
#include <fstream>
#include <string>
#include "ioManager.h"
#include <vector>

void ioManager::readInputFile() {
	// declare necessary variables
	string line;
	string fileName = inputPath;
	ifstream inputFile(fileName);
	vector<vector<string>> inputOpts;
	vector<string> delimLine;
	ioManager::log("Opening input file " + inputPath, 0, 1);

	// read input txt line by line, separate lines by equal sign
	int i = 0;
	if (inputFile.is_open()) {
		while (getline(inputFile, line)) {
			delimLine = split(line, '=');
			inputOpts.push_back(delimLine);
		}
		inputFile.close();
	}
	else {
		ioManager::log("Unable to open file: " + fileName, 0, 1);
	}

	
	//TODO do all of this in more appropriate methods (especially the output settings)
	// parse txt for variables, potential and time parameters
	ioManager::log("Reading input TXT... ",0,0);
	for (int i = 0; i < inputOpts.size(); i++) {
		vector<string> opt = inputOpts[i];
		if (opt.size() != 2) {
			ioManager::log("Skipped line " + opt[0] + " ...",0,0);
			continue;
		}
		if (opt[0].compare("xml") == 0) {
			xml = opt[1];
			ioManager::log("XML path found... ",0,0);
			continue;
		}
		if (opt[0].compare("csv") == 0) {
			csv = opt[1];
			ioManager::log("CSV path found... ",0,0);
			csv_length = checkCsvFile();
			continue;
		}
		if (opt[0].compare("var") == 0) {
			var = opt[1];
			ioManager::log("variable found... ",0,0);
			continue;
		}
		if (opt[0].compare("phi") == 0) {
			phi = opt[1];
			ioManager::log("potential found... ",0,0);
			continue;
		}
		if (opt[0].compare("steps") == 0) {
			noStep = stoi(opt[1]);
			ioManager::log("number of steps found... ",0,0);
			continue;
		}
		if (opt[0].compare("t_end") == 0) {
			endTime = opt[1];
			ioManager::log("simulation time found... ",0,0);
			continue;
		}
		if (opt[0].compare("dt_max") == 0) {
			maxTimeStep = opt[1];
			ioManager::log("maximum time step found... ",0,0);
			continue;
		}
		if (opt[0].compare("dt_min") == 0) {
			minTimeStep = opt[1];
			ioManager::log("minimum time step found... ",0,0);
			continue;
		}
		if (opt[0].compare("dt_init") == 0) {
			initTimeStep = opt[1];
			ioManager::log("initial time step found... ",0,0);
			continue;
		}
		if (opt[0].compare("solv_rtol") == 0) {
			solverRelativeTolerance = opt[1];
			ioManager::log("solver relative tolerance found... ", 0, 0);
			continue;
		}
		if (opt[0].compare("solv_atol") == 0) {
			solverAbsoluteTolerance = opt[1];
			ioManager::log("solver absolute tolerance found... ", 0, 0);
			continue;
		}
		if (opt[0].compare("ss_crit") == 0) {
			steadyStateCriterion = opt[1];
			ioManager::log("steady-state criterion found... ", 0, 0);
			continue;
		}
		if (opt[0].compare("t_avg") == 0) {
			finalMeanDuration = opt[1];
			ioManager::log("time interval for averaging final value found... ", 0, 0);
			continue;
		}
		if (opt[0].compare("n_avg") == 0) {
			finalMeanSteps = stoi(opt[1]);
			ioManager::log("number of steps for averaging final value found... ", 0, 0);
			continue;
		}
		if (opt[0].compare("output") == 0) {
			outputPath = opt[1];
			ioManager::log("output path found...",0,0);
			continue;
		}
		if (opt[0].compare("interaction") == 0) {
			requireInteraction = stringToBool(opt[1]);
			ioManager::log("interaction option found...",0,0);
			continue;
		}
		if (opt[0].compare("products") == 0) {
			productNames = split(opt[1], ',');
			ioManager::log("product names found...",0,0);
			continue;
		}
		if (opt[0].compare("mode") == 0) {
			if (opt[1].compare("ALL") == 0) {
				fileOutputOpts = OUTPUT_ALL;
			}
			else {
				if (opt[1].compare("CUMULATED") == 0) {
					fileOutputOpts = OUTPUT_CUMULATED;
				}
				else {
					if (opt[1].compare("TRANSIENT") == 0) {
						fileOutputOpts = OUTPUT_TRANSIENT;
					}
					else {
						if (opt[1].compare("NONE") == 0) {
							fileOutputOpts = OUTPUT_NONE;
						}
						else {
							throw("error");
						}
					}
				}
			}
			ioManager::log("output mode found...",0,0);
			continue;
		}
		if (opt[0].compare("filename") == 0) {
			if (opt[1].compare("TIMESTAMP") == 0) {
				nameOutputOpts = FILENAME_TIMESTAMP;
			}
			else {
				if (opt[1].compare("OUTPUT") == 0) {
					nameOutputOpts = FILENAME_OUTPUT;
				}
				else {
					throw("error");
				}
			}
			ioManager::log("output filename found...",0,0);
			continue;
		}
		if (opt[0].compare("species") == 0) {
			if (opt[1].compare("ALL") == 0) {
				specOutputOpts = SPECIES_ALL;
			}
			else {
				if (opt[1].compare("RATES") == 0) {
					specOutputOpts = SPECIES_RATES;
				}
				else {
					if (opt[1].compare("NONE") == 0) {
						specOutputOpts = SPECIES_NONE;
					}
					else {
						throw("error");
					}
				}
			}
			ioManager::log("species output options found...",0,0);
			continue;
		}
		if (opt[0].compare("reactions") == 0) {
			if (opt[1].compare("ALL") == 0) {
				rxnOutputOpts = REACTION_ALL;
			}
			else {
				if (opt[1].compare("RATES") == 0) {
					rxnOutputOpts = REACTION_RATES;
				}
				else {
					if (opt[1].compare("NET") == 0) {
						rxnOutputOpts = REACTION_NET;
					}
					else {
						if (opt[1].compare("NONE") == 0) {
							rxnOutputOpts = REACTION_NONE;
						}
						else {
							throw("error");
						}
					}
				}
			}
			ioManager::log("reaction output options found...",0,0);
			continue;
		}
		if (opt[0].compare("coverage") == 0) {
			if (stringToBool(opt[1])) {
				covOutputOpts = COVERAGE_ON;
			}
			else {
				covOutputOpts = COVERAGE_OFF;
			}
			ioManager::log("coverage output options found...",0,0);
			continue;
		}
		if (opt[0].compare("sampling") == 0) {
			if (opt[1].compare("T10") == 0) {
				sampleOutputOpts = SAMPLING_T10;
			}
			else {
				if (opt[1].compare("NONE") == 0) {
					sampleOutputOpts = SAMPLE_NONE;
				}
				else {
					throw("error");
				}
			}
			ioManager::log("sampling output options found...",0,0);
			continue;
		}
		if (opt[0].compare("current") == 0) {
			if (opt[1].compare("ALL") == 0) {
				currOutputOpts = CURRENT_ALL;
			}
			else {
				if (opt[1].compare("FE") == 0) {
					currOutputOpts = CURRENT_FE;
				}
				else {
					if (opt[1].compare("SPEC") == 0) {
						currOutputOpts = CURRENT_SPEC;
					}
					else {
						if (opt[1].compare("NONE") == 0) {
							currOutputOpts = CURRENT_NONE;
						}
						else {
							throw("error");
						}
					}
				}
			}
			ioManager::log("coverage output options found...",0,0);
			continue;
		}
		if (opt[0].compare("changerate") == 0) {
			if (opt[1].compare("NONE") == 0) {
				ddtOutputOpts = DDT_NONE;
			}
			else {
				if (opt[1].compare("SS") == 0) {
					ddtOutputOpts = DDT_SS;
				}
				else {
					if (opt[1].compare("PROD") == 0) {
						ddtOutputOpts = DDT_PROD;
					}
					else {
						if (opt[1].compare("ALL") == 0) {
							ddtOutputOpts = DDT_ALL;
						}
						else {
							throw("error");
						}
					}
				}
			}
			ioManager::log("changerate output options found...",0,0);
			continue;
		}
		if (opt[0].compare("logging") == 0) {
			if (opt[1].compare("NONE") == 0) {
				logOutputOpts = LOG_NONE;
			}
			else {
				if (opt[1].compare("TXT") == 0) {
					logOutputOpts = LOG_TXT;
				}
				else {
					if (opt[1].compare("INPUT") == 0) {
						logOutputOpts = LOG_INPUT;
					}
					else {
						if (opt[1].compare("ALL") == 0) {
							logOutputOpts = LOG_ALL;
						}
						else {
							throw("error");
						}
					}
				}
			}
			ioManager::log("logging output options found...",0,0);
			continue;
		}
	}

	ioManager::log("", 1, 1);

}

void ioManager::copyFile(string in, string out) {
	std::ifstream inFile(in);
	std::ofstream outFile(out);

	outFile << inFile.rdbuf();
}

//TODO create set functions that take string arguments and therefore replace nested conditions above

int ioManager::checkCsvFile() {
	int counter = 0;
	string line;
	ifstream csvFile(csv);
	if (csvFile.is_open()) {
		while (getline(csvFile, line)) {
			++counter;
		}
	}
	else {
		ioManager::log("Unable to open file: " + csv, 0, 1);
		return -1;
	}

	return counter;
}

double ioManager::getCsvVal(string var, int row) {
	if (row >= csv_length) {
		ioManager::log("Row " + to_string(row) + " does not exist in " + csv, 0, 1);
		return -1.0;
	}

	if (row < 0) {
		row = csv_length - 1;
	}

	ioManager::log("Reading line " + to_string(row) + " of variable " + var + " from " + csv, 0, 1);

	string line;
	vector<string> line_cols;
	int col;
	bool rowFound = false;
	ifstream csvFile(csv);
	int i = 0;

	while (getline(csvFile, line)) {
		if (i == 0) {
			line_cols = split(line, ',');
			auto it = std::find(line_cols.begin(), line_cols.end(), var);
			if (it != line_cols.end()) {
				col = std::distance(line_cols.begin(), it);
			}
			else {
				ioManager::log(" Variable not found!", 0, 1);
				return 0.0;
			}
		}

		if (i == row) {
			line_cols = split(line, ',');
			break;
		}
		i++;
	}
	csvFile.close();

	return stof(line_cols[col]);

}


double* ioManager::getCsvCol(string var) {
	if (csv_length < 2) {
		ioManager::log("File " + csv + " does not have enough rows!", 0, 1);
		return new double(0);
	}
	
	ioManager::log("Reading column " + var + " from " + csv + " ... ", 0, 1);
	double *varArr = new double[csv_length - 1];

	string line;
	vector<string> line_cols;
	int col;
	bool rowFound = false;
	ifstream csvFile(csv);
	int i = 0;

	while (getline(csvFile, line)) {
		// line_cols.clear();
 		line_cols = split(line, ',');
		if (i == 0) {
			auto it = std::find(line_cols.begin(), line_cols.end(), var);
			if (it != line_cols.end()) {
				col = std::distance(line_cols.begin(), it);
			}
			else {
				ioManager::log(" Variable not found!", 0, 1);
				return new double(0);
			}
		}
		else {
			varArr[i - 1] = stof(line_cols[col]);
		}
		i++;
	}
	csvFile.close();

	return varArr;

}

bool ioManager::stringToBool(string s) {
	if (s.compare("YES") == 0) {
		return true;
	}
	else {
		if (s.compare("NO") == 0) {
			return false;
		}
		else {
			throw("error");
		}
	}
}