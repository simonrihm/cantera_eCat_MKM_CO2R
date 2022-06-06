#include <string>
#include "varManager.h"
#include "ioManager.h"

// construct function with default values
varManager::varManager() {
	y_out = 0;
	x_min = 0;
	x_max = 1;
	y_a = 1;
	y_b = 0;

}


// calculate function value y=f(x)
double varManager::varFun(double x) {
	double y;
	if (x <= x_max && x >= x_min) {
		switch (funMode)
		{
		case FUNMODE_CONSTANT:
			// just pass through
		case FUNMODE_LINEAR:
			y = y_a + y_b * x;
			break;
		case FUNMODE_CUSTOM:
			y = interpCustom(x);
			break;
		default:
			break;
		}
	}
	else {
		y = y_out;
	}
	return y;
}

// interpolate between values of custom function
double varManager::interpCustom(double x) {
	varManager interp = varManager();
	for (int i = 0; i < lengthCustom; i++) {
		if (x == x_custom[i]) {
			return y_custom[i];
		}
		if (x < x_custom[i]) {
			interp.setLim(x_custom[i - 1], x_custom[i]);
			interp.setLin(y_custom[i - 1], y_custom[i]);
			return interp.varFun(x);
		}
	}
	return y_custom[lengthCustom - 1];
}

// set function input limits
void varManager::setLim(double x1, double x2) {
	x_min = x1;
	x_max = x2;
}

// set function value outside input limits
void varManager::setFallback(double y) {
	y_out = y;
}

// set function to a constant y=c
void varManager::setConst(double y) {
	y_a = y;
	y_b = 0;
	funMode = FUNMODE_CONSTANT;
}

// set function to linear sweep from f(x_min)=y1 to f(x_max)=y2
void varManager::setLin(double y1, double y2) {
	y_a = y1;
	y_b = (y2 - y1) / (x_max - x_min);
	y_a = y1 - y_b * x_min;
	funMode = FUNMODE_LINEAR;
}

// set function to interpolation of given values
void varManager::setCustom(string y, string x) {
	if (std::isdigit(x[x.length()-1])) {
		setConst(ioManager::getCsvVal(y, stoi(x)));
	}
	else {
		x_custom = ioManager::getCsvCol(x);
		y_custom = ioManager::getCsvCol(y);
		lengthCustom = ioManager::getCsvLength();
		funMode = FUNMODE_CUSTOM;
	}
	
}

// read string input and set function accordingly
void varManager::readFun(string s) {
	vector<string> s_mode = ioManager::split(s, '(');
	s_mode[1].resize(s_mode[1].size() - 1);
	vector<string> s_params;

	if (s_mode[0].compare("const") == 0) {
		setConst(stof(s_mode[1]));
	}
	else if (s_mode[0].compare("lin") == 0) {
		s_params = ioManager::split(s_mode[1], ',');
		setLin(stof(s_params[0]), stof(s_params[1]));
	}
	else if (s_mode[0].compare("csv") == 0) {
		s_params = ioManager::split(s_mode[1], ',');
		setCustom(s_params[0], s_params[1]);
		//setConst(ioManager::getCsvInput(s_params[0], stoi(s_params[1])));
		//TODO make it so that CSV can not only read const from specific line but a function defined by multiple lines
	}
}