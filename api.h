#ifndef   API_H
#define   API_H

#include <iostream>
#include "json/json.hpp"
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>

#include <unistd.h>
#include "curl/curl.h"

#ifndef  COMMON_H  
#include "common.h"
#endif
#ifndef  CONFIGURATION_H  
#include "configuration.h"
#endif
#ifndef  HTTP_H  
#include "http.h"
#endif

using namespace std;

class User {


public:
	string username;
	string password;

	User(string u, string p) {
		username = u;
		password = p;
	}
};



string genRandom(const int len) {

	string tmp_s;
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	srand((unsigned)time(NULL) * getpid());

	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i)
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];


	return tmp_s;

}

bool registerUser(User user, string processFileName) {
	const string postData = "_username=" + escape_json(user.username) + "&_password=" + escape_json(user.password) + "";

	curlPost(apiUrl + "/register", postData, "", false);

	writeAPiCredentials(user.username, user.password, processFileName);


	return true;
}


string requestToken(string processFileName, bool alreadyRegistered = false) {

	string token = getToken(processFileName);
	if (!token.empty()) {
		return token;
	}

	string apiCredentials = getApiCredentials(processFileName);

	string username;
	string password;

	if (apiCredentials.empty()) {
		username = genRandom(26);
		password = genRandom(12);
		registerUser(User(username, password), processFileName);
	}
	else {

		stringstream credentials(apiCredentials);
		string segment;
		vector<string> seglist;

		while (getline(credentials, segment, ':'))
		{
			seglist.push_back(segment);
		}

		username = seglist.at(0);
		password = seglist.at(1);
	}

	const string json = "{\"username\":\"" + escape_json(username) + "\", \"password\":\"" + escape_json(password) + "\"}";

	string returnVal = curlPost(apiUrl + "/login_check", json, "");

	nlohmann::json jsonData = nlohmann::json::parse(returnVal);

	int code = 200;
	if (!jsonData["code"].empty()) {
		code = jsonData["code"];
	}

	if (code == 200) {
		
		string token = jsonData["token"];
		return token;
	}
	if (code == 401) {
		registerUser(User(genRandom(25), genRandom(12)), processFileName);

		requestToken(processFileName, true);
	}
	

	return "";
}

#endif