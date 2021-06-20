#include <iostream>
#include <filesystem>
#include <chrono>
#include <string>
#include <regex>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "curl/curl.h"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <sys/stat.h>
#include <fstream>
#include <limits>

#ifndef  HTTP_H  
#include "http.h"
#endif
#ifndef  CONFIGURATION_H  
#include "configuration.h"
#endif
#include "file.h"
#ifndef  COMMON_H  
#include "common.h"
#endif
#ifndef  SERVER_H  
#include "server.h"
#endif
#ifndef  API_H  
#include "api.h"
#endif

namespace fs = std::filesystem;

using namespace std;




bool comparator(string a, string b)
{
	return a > b;
}


vector<string> getFilesInDir(string dir) {

	vector<string> names;

	for (const auto& entry : fs::directory_iterator(dir))
	{
		names.push_back(entry.path());
	}

	sort(names.begin(), names.end(), comparator);

	return names;
}



string filterFirstLogFile(string logDir) {

	vector<string> names = getFilesInDir(logDir);

	for (unsigned int a = 0; names.size() > a; a++)
	{
		if (regex_search(names.at(a), regex("Server_CryoFall", regex_constants::ECMAScript | regex_constants::icase))) {
			return names.at(a);
		}
	}

	return "";
}

void postLine(string line, string processFileName) {

	string serverGuid = getServerUid(processFileName);
	if (serverGuid.length() == 0) {
		cout << "Missing Server GUID. Please restart script." << endl;
		return;
	}

	const string json = "{\"GUID\":\"" + serverGuid + "\", \"line\":\"" + escape_json(line) + "\"}";

	string token = requestToken(processFileName);
	curlPost(apiUrl + "/raw/line", json, token);
}


void parseAndRequestLine(string line, string processFileName) {

	if (line.find("Registering as a public server on the Master Server") != string::npos || line.find("Registering as a private server on the Master Server") != string::npos) {

		regex e("(.*)GUID=(.*)");
		cmatch cm;
		regex_match(line.c_str(), cm, e, regex_constants::match_default);

		cout << "GUID:" << line << endl;

		writeServerGuid(cm[2], processFileName);
	}

	if (line.find("[SCRIPT] Character killed") != string::npos
		|| line.find("[SCRIPT] Character dead") != string::npos
		|| line.find("[SCRIPT] ChatId") != string::npos
		|| line.find("[CHAT]") != string::npos
		|| line.find("players_online_count") != string::npos
		|| line.find("scopes are enabled") != string::npos
		|| line.find("ping game=") != string::npos
		|| line.find("scopes are disabled") != string::npos) {
		cout << "\nPOST:" << line << endl;
		postLine(line, processFileName);
	}
}

void parseFile(string file, int lastPosition, string processFileName) {
	fstream newfile;
	newfile.open(file, ios::in);


	if (newfile.is_open()) {
		string tp;
		for (int n = 1; n < lastPosition; n += 1) {
			if (!getline(newfile, tp)) {
				// Error or premature end of file! Handle appropriately.
			}
		}
		string chatFrom = "";
		string tmpChat = "";

		while (getline(newfile, tp)) {
			cout << '\r' << ". Parse line " << lastPosition << "..." << flush;

			if (tp.find("[SCRIPT] ChatId") != string::npos) {
				tmpChat = tp;
				lastPosition++;
				continue;
			}

			if (tmpChat.length() > 0) {
				chatFrom = tmpChat + " " + tp;
				tmpChat = "";
				lastPosition++;
				continue;
			}

			if (tp.find("Character") != string::npos && !chatFrom.empty()) {
				tp = chatFrom + " " + tp;
				chatFrom = "";
			}

			parseAndRequestLine(tp, processFileName);

			lastPosition++;
		}
	}

	newfile.close();

	if (lastPosition > 0) {
		writeCurrentPosition(lastPosition, processFileName);
	}

	if (file.length() > 0) {
		writeCurrentLogFile(file, processFileName);
	}
	this_thread::sleep_for(chrono::milliseconds(2000));
}

