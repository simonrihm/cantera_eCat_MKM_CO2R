#include "ioManager.h"
#include <ctime>
#include <sstream>
#include <iomanip>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>


ioManager::ioManager() {
}

string ioManager::createTimeStamp() {
	time_t ts = time(nullptr);
	stringstream ss;
	ss << put_time(localtime(&ts), "%F_%H-%M-%S");
	return ss.str();
}

string ioManager::getOutputPath() {
	if (outputPath.compare("_") == 0 || outputPath.compare("") == 0) {
		return "";
	}
	else {
		return outputPath + "/";
	}
}


string ioManager::exec(const char* cmd) {
	std::array<char, 128> buffer;
	string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}