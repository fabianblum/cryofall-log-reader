#ifndef   UPDATE_H
#define   UPDATE_H

#include <iostream>
#include <filesystem>
#include "curl/curl.h"
#include <sstream>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <sys/stat.h>

#ifndef  HTTP_H  
#include "http.h"
#endif

using namespace std;

namespace fs = std::filesystem;

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

int getOnlineVersion() {
	string response_string = curlGet("https://cryofall-api.com/version.txt");

	if (response_string.length() <= 0) {
		response_string = "0";
	}

	return stoi(response_string);
}

bool isUpdateAvailable(int version, const char* processFileName) {
	int onlineVersion = getOnlineVersion();

	return onlineVersion > version;
}

void doUpdate(int version, const char* processFileName) {
	cout << ". Checking for update ..." << endl;
	if (!isUpdateAvailable(version, processFileName)) {
		cout << ". No update. Exit." << endl;
		return;
	}

	char newFilename[256];

	const char* prefix = "_";

	string newFileName, oldFileName;
	string currentFileName = fs::path(processFileName).filename();
	string currentPath = fs::current_path();

	oldFileName = currentPath + "/" + currentFileName;
	newFileName = currentFileName + "_old";

	remove(newFileName.c_str());
	if (rename(oldFileName.c_str(), newFileName.c_str()) != 0) {
		throw runtime_error("Could not rename file for update process");
	}

	cout << ". Update file ..." << endl;
	CURL* curl;
	FILE* fp;
	CURLcode res;
	string url = "https://cryofall-api.com/files/LogReader";
	curl = curl_easy_init();
	if (curl) {
		fp = fopen(oldFileName.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
	}

	cout << ". Cleaning up..." << endl;
	chmod(oldFileName.c_str(), 0744);
	remove(newFileName.c_str());


	cout << "Update done. Please restart." << endl;
	exit(0);
}

void checkUpdate(int version, const char* processFileName) {
	if (isUpdateAvailable(version, processFileName)) {
		cout << "Update avaialble. Please run \"LogReader --self-update\". Script starts in 5 seconds" << "\n\n";
		this_thread::sleep_for(chrono::milliseconds(5000));
	}
}

#endif