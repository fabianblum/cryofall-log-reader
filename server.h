#ifndef   SERVER_H
#define   SERVER_H

#include <fstream>
#ifndef  COMMON_H  
#include "common.h"
#endif

using namespace std;

string getServerUid(string processFileName)
{
	string line = "";
	ifstream myfile(processFileName + ".cnf");
	if (myfile.is_open())
	{
		getline(myfile, line);
		myfile.close();
	}

	return line;
}

#endif