#include <iostream>

#include <cantera/base/ctexceptions.h>

//#include "Errors.h"

#include "Cat_reactor.h"

#include "ioManager.h"

#include "simManager.h"

#include <filesystem>

#include <csignal>

using namespace std;
bool timeoutFlag;

void signalHandler(int signum) {
	ioManager::log("Timeout signal (signum " + to_string(signum) + ") received.", 0, 1);
	timeoutFlag = true;
}

//TODO allocate stuff on the stack whenever possible

int main(int argc, char *argv[])
{
	timeoutFlag = false;
	signal(SIGINT, signalHandler);

	try
	{
		// current directory
		wchar_t* buffer;
		buffer = _wgetcwd(NULL, 0);
		wstring ws(buffer);
		string s(ws.begin(), ws.end());
		ioManager::log("Operating in directory " + s, 0, 2);

		ioManager::setDefaultValues();
		
		// read command line arguments
		if (argc > 1) {
			ioManager::setInputPath(argv[1]);
		}
		if (argc > 2) {
			ioManager::exec(argv[2]);
		}
		/*if (argc > 3) {
			ioManager::setOutputPath(argv[3]);
		}
		if (argc > 4) {
			ioManager::setReqInteract(argv[4]);
		}*/

		// read and run mechanism
		ioManager::readInputFile();
		simManager::initModel();
		simManager::runSim();

		// if interaction is not deactivated...
		if (ioManager::getReqInteract()) {
			system("pause");
		}
		
		// close open windows
		Cantera::appdelete(); // delete cantera app
		return 0;
	}
	catch (Cantera::CanteraError& err)
	{
		ioManager::log("Error encountered: " + err.getMessage(), 0, 1);
		system("pause");
		return 1;		// stop here and examine "err" for debugging
	}
}

