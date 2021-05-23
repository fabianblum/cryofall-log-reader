#include <iostream>

using namespace std;

string getHash(string stringToHash) {
	size_t h1 = hash<string>{}(stringToHash);

	return to_string(h1);
}