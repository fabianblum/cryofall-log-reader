#include <iostream>
#include "curl/curl.h"

using namespace std;

static size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

string curlGet(string url) {

	CURLcode ret;
	CURL* hnd;
	struct curl_slist* slist1;

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(hnd, CURLOPT_FORBID_REUSE, 1L);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.38.0");
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

	string response_string;
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writeFunction);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response_string);


	curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);
	hnd = NULL;


	return response_string;
}

void curlPost(string url, string postData) {
	CURLcode ret;
	CURL* hnd;
	struct curl_slist* slist1;

	slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postData.c_str());
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE, postData.size());
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.38.0");
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");

	ret = curl_easy_perform(hnd);
	cout << "POST Result " << ret;

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_slist_free_all(slist1);
	slist1 = NULL;
}