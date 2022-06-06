#pragma once
#include <string>
using namespace std;

class varManager
{
	public:
		varManager();
		double varFun(double x);
		void setLim(double x1, double x2);
		void setFallback(double y);
		void setConst(double y);
		void setLin(double y1, double y2);
		void setCustom(string y, string x);
		void readFun(string s);
		int getFunMode() { return funMode; };
		double interpCustom(double x);

		// function mode options: funMode
		static const int FUNMODE_CONSTANT = 0;
		static const int FUNMODE_LINEAR = 1;
		static const int FUNMODE_CUSTOM = 2;
	private:
		double x_min;
		double x_max;
		double y_a;
		double y_b;
		double y_out;

		int funMode;

		double* x_custom;
		double* y_custom;
		int lengthCustom;
};

