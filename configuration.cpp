#include <iostream>
#include "file.h"

using namespace std;

void writeConfigFile(string serverGuid, string processFileName) {
	string cnfFile = processFileName + ".cnf";

	writeToFile(cnfFile.c_str(), 0, serverGuid.c_str());
}