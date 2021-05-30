// LogReaderCmake.cpp: Definiert den Einstiegspunkt für die Anwendung.
//
#include <iostream>
#include <filesystem>
#include "LogFileParser.h"
#include <signal.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/prctl.h>
#include<signal.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#ifndef  COMMON_H  
#include "common.h"
#endif
#ifndef  LOCK_H  
#include "lock.h"
#endif
#ifndef  FILE_H  
#include "file.h"
#endif
#ifndef  SERVER_H  
#include "server.h"
#endif
#ifndef  API_H  
#include "api.h"
#endif
#ifndef  UPDATE_H  
#include "update.h"
#endif





using namespace std;
static int version = 2;
const char* processFileName;

void my_handler(int s) {
	unlockProcess(processFileName);
	exit(1);

}


int main(int argc, char** argv) {

	string token = requestToken("fblum", "test");

	return 0;
	
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	processFileName = argv[0];

	cleanLockFile(processFileName);
	if (!isOnlyInstance(processFileName)) {
		std::cout << "Another process is running. Only one process per server is allowed." << std::endl;
		exit(0);
	}
	lockProcess(processFileName);
	
	bool autoUpdateEnabled = false;
	string logDir = "";

	vector<string> args(argv, argv + argc);
	for (size_t i = 1; i < args.size(); ++i) {
		if (args[i] == "--self-update") {
			cout << "Do Update" << "\n";
			doUpdate(version, processFileName);
			exit(0);
		}

		if (args[i] == "--auto-update") {
			cout << "Auto update enabled" << "\n";
			autoUpdateEnabled = true;
			doUpdate(version, processFileName);
		}

		if (args[i] == "-v") {
			cout << "Version " << version << "\n";
			exit(0);
		}

		if (args[i].rfind("--log-dir=", 0) == 0) {
			logDir = args[i].substr(args[i].find("=") + 1);
		}

		if (args[i] == "--auto-update") {
			cout << "Auto update enabled" << "\n";
			autoUpdateEnabled = true;
			doUpdate(version, processFileName);
		}
	}

	if (logDir.length() == 0) {
		cout << "Please define a log directory \"--log-dir=/dir/to/cryofall/server/log\"" << "\n";
		exit(0);
	}

	if (false == autoUpdateEnabled) {
		checkUpdate(version, processFileName);
	}

	cout << "Start searching for newest LogFile in directory \"" << logDir << "\"" << endl;

	auto start = chrono::system_clock::now();

	string lastFileName = "";
	for (;;) {
		string file = filterFirstLogFile(logDir);
		int lastPosition = getLastPosition(file, processFileName);

		if (file.length() == 0) {
			cout << "\n" << "No logging file found. Exit." << "\n";
			return 0;
		}

		if (lastFileName != file) {
			cout << "\n" << "Start parsing file \"" << file << "\" beginning with line \"" << lastPosition << "\"" << endl;
		}
		lastFileName = file;
		parseFile(file, lastPosition, processFileName);

		auto current = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = current - start;

		if (elapsed_seconds.count() > 600 && autoUpdateEnabled) {
			doUpdate(version, processFileName);
			start = chrono::system_clock::now();
		}
	}

	unlockProcess(processFileName);
	return 0;
}