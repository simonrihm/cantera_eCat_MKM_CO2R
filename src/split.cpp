#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include "ioManager.h"

vector<string> ioManager::split(string s, char delimiter)
{
	vector<string> tokens;
	string token;
	stringstream tokenStream(s);
	while (getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	//tokenStream.clear();
	//tokenStream.str("");

	return tokens;
}