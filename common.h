#ifndef   COMMON_H
#define   COMMON_H

#include <iostream>
#include <iomanip>

using namespace std;

string apiUrl = "https://beta.cryofall-api.com";

string getHash(string stringToHash) {
	size_t h1 = hash<string>{}(stringToHash);

	return to_string(h1);
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
#endif