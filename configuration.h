#ifndef   CONFIGURATION_H
#define   CONFIGURATION_H

#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <filesystem>
#include <fstream>

#ifndef  COMMON_H  
#include "common.h"
#endif
#ifndef  FILE_H  
#include "file.h"
#endif

using namespace std;

const char* getHomeDir() {
	const char* homedir;

	if ((homedir = getenv("HOME")) == NULL) {
		homedir = getpwuid(getuid())->pw_dir;
	}

	return homedir;
}

string getConfigFile(string processFileName)
{
	string cnfFileName;
	cnfFileName += getHomeDir();
	cnfFileName += "/." + processFileName + ".cnf";

	return cnfFileName;
}

void writeServerGuid(string serverGuid, string processFileName) {
	string cnfFile = getConfigFile(processFileName);

	writeToFile(cnfFile.c_str(), 0, serverGuid.c_str());
}

void writeCurrentPosition(int position, string processFileName) {
	string cnfFile = getConfigFile(processFileName);

	writeToFile(cnfFile.c_str(), 1, to_string(position).c_str());
}

void writeCurrentLogFile(string file, string processFileName) {
	string cnfFile = getConfigFile(processFileName);

	writeToFile(cnfFile.c_str(), 2, getHash(file).c_str());
}

void writeAPiCredentials(string username, string password, string processFileName) {
	string cnfFile = getConfigFile(processFileName);

	string credentials = username + ":" + password;

	writeToFile(cnfFile.c_str(), 3, credentials.c_str());
}

void writeToken(string token, string processFileName) {
	string cnfFile = getConfigFile(processFileName);

	writeToFile(cnfFile.c_str(), 4, token.c_str());
}




string getFromLine(string processFileName, int neededLine) {

	string cnfFile = getConfigFile(processFileName);

	ifstream input(cnfFile);
	if (input.fail()) {
		return "";
	}

	string line;

	string newFile;
	unsigned position = 0;
	for (string line; getline(input, line); position++)
	{
		if (position == neededLine) {
			return line;
		}
	}

	return "";
}

string getServerGuid(string processFileName)
{
	return getFromLine(processFileName, 0);
}

int getLastPosition(string currentFileName, string processFileName)
{
	return stoi(getFromLine(processFileName, 1));
}

string getLogHash(string processFileName)
{
	return getFromLine(processFileName, 2);
}

string getApiCredentials(string processFileName)
{
	return getFromLine(processFileName, 3);
}

string getToken(string processFileName)
{
	return getFromLine(processFileName, 4);
}

#endif