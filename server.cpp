#include <fstream>
#include "common.h"

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



int getLastPosition(string currentFileName, string processFileName)
{
	ifstream input(processFileName+".cnf");
	if (input.fail()) {
		return 0;
	}
	string line;
	int last_position = 0;

	string newFile;
	unsigned position = 0;
	for (string line; getline(input, line); position++)
	{
		if (position == 1) {
			last_position = stoi(line);
		}
		if (position == 2 && getHash(currentFileName) != line) {
			last_position = 0;
		}
	}

	return last_position;
}
