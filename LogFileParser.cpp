#include "LogReaderCmake.h"
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
#include "http.h"
#include "configuration.h"
#include "file.h"
#include "common.h"
#include "server.h"



namespace fs = std::filesystem;

using namespace std;




bool comparator(string a, string b)
{
	return a > b;
}

string escape_json(const string& s) {
	ostringstream o;
	for (auto c = s.cbegin(); c != s.cend(); c++) {
		switch (*c) {
		case '"': o << "\\\""; break;
		case '\\': o << "\\\\"; break;
		case '\b': o << "\\b"; break;
		case '\f': o << "\\f"; break;
		case '\n': o << "\\n"; break;
		case '\r': o << "\\r"; break;
		case '\t': o << "\\t"; break;
		default:
			if ('\x00' <= *c && *c <= '\x1f') {
				o << "\\u"
					<< std::hex << std::setw(4) << std::setfill('0') << (int)*c;
			}
			else {
				o << *c;
			}
		}
	}
	return o.str();
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
		throw runtime_error("Missing Server GUID. Please restart script.");
	}

	const string json = "{\"GUID\":\"" + serverGuid + "\", \"line\":\"" + escape_json(line) + "\"}";

	curlPost("https://cryofall-api.com/api.php", json);
}


void parseAndRequestLine(string line, string processFileName) {

	if (line.find("[IMP] Registering as a private server on the Master Server") != string::npos) {

		regex e("(.*)GUID=(.*)");
		cmatch cm;
		regex_match(line.c_str(), cm, e, regex_constants::match_default);

		cout << "GUID:" << line << endl;

		writeConfigFile(cm[2], processFileName);
	}

	if (line.find("[SCRIPT] Character killed") != string::npos
		|| line.find("[SCRIPT] Creating loot") != string::npos
		|| line.find("[SCRIPT] Character dead") != string::npos
		|| line.find("[SCRIPT] ChatId") != string::npos) {
		cout << "\nPOST:" << line << endl;
		postLine(line, processFileName);
	}
}

void parseFile(string file, int lastPosition, string processFileName) {
	fstream newfile;
	newfile.open(file, ios::in);
	string cnfFileName = processFileName + ".cnf";


	if (newfile.is_open()) {
		string tp;
		for (int n = 1; n < lastPosition; n += 1) {
			if (!getline(newfile, tp)) {
				// Error or premature end of file! Handle appropriately.
			}
		}

		while (getline(newfile, tp)) {
			cout << '\r' << ". Parse line " << lastPosition << "..." << flush;
			parseAndRequestLine(tp, processFileName);
			lastPosition++;
		}
	}

	newfile.close();

	if (lastPosition > 0) {
		writeToFile(cnfFileName.c_str(), 1, to_string(lastPosition).c_str());
	}

	if (file.length() > 0) {
		writeToFile(cnfFileName.c_str(), 2, getHash(file).c_str());
	}
	this_thread::sleep_for(chrono::milliseconds(2000));
}

