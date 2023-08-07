#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

struct Parser {
	static vector<string> splitStringD(string& input, string delimiter) {
		vector<string> tokens;
		string::size_type start = 0;
		string::size_type end = input.find(delimiter);

		while (end != string::npos) {
			tokens.push_back(input.substr(start, end - start));
			start = end + 1;
			end = input.find(delimiter, start);
		}
		tokens.push_back(input.substr(start));

		return tokens;
	}

	static vector<string> splitString(string& input) {
		vector<string> result;
		istringstream iss(input);
		string token;

		while (iss >> token) {
			result.push_back(token);
		}

		return result;
	}
};

struct Triangle {
	size_t i1;
	size_t i2;
	size_t i3;
};

class Mesh;

class Mesh {
public:
	vector<glm::vec3> vertices;
	vector<Triangle> faces;

	static Mesh readObj(string filepath);
};