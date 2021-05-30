#ifndef   FILE_H
#define   FILE_H

#include <iostream>
#include <fstream>

using namespace std;

void writeToFile(char const* filename, unsigned lineNo, char const* toWrite)
{
	FILE* fileToCreate;
	if (fileToCreate = fopen(filename, "r")) {
		fclose(fileToCreate);
	}
	else {
		fileToCreate = fopen(filename, "w+b");
		fclose(fileToCreate);
	}
	fileToCreate = NULL;


	ifstream input(filename);
	string line;

	string newFile;
	unsigned position = 0;
	for (string line; getline(input, line); position++)
	{

		if (position != lineNo) {
			newFile.append(line + "\n");
			continue;
		}

		newFile.append(toWrite);
		newFile.append("\n");
	}

	for (; position <= lineNo; position++)
	{

		if (position == lineNo) {
			newFile.append(toWrite);
		}

		newFile.append("\n");
	}


	ofstream myfile(filename);
	if (myfile.is_open())
	{
		myfile << newFile;
		myfile.close();
	}
	else cout << "Unable to open file";
}

#endif