#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace glm;

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
	size_t i1, i2, i3 ,u1, u2, u3 ,n1 ,n2, n3;
	Triangle(){
		i1 = 0; i2 = 0; i3 = 0;
		u1 = -1; u2 = -1; u3 = -1;
		n1 = -1; n2 = -1; n3 = -1;
	}
};

struct Vertex {
	vec3 pos;
	vec3 col;
	vec3 normal;
	vec2 texcoord;

	Vertex() {
		pos = vec3(0);
		col = vec3(1);
		normal = vec3(1);
		texcoord = vec2(0);
	}
};

class Mesh;

class Mesh {
public:
	vec3 rot, pos, scale;

	vector<glm::vec3> vertices;
	vector<vec3> vertex_colors;
	vector<vec3> vertex_normals;
	vector<vec2> vertex_uvs;
	vector<Triangle> faces;

	mat4x4 model_matrix;
	vector<Vertex> vertex_output;

	Mesh();

	void processMatrix();
	void processVertices(const mat4x4& cam_mat, const mat4x4& proj_mat, const mat4x4& view_mat);

	static Mesh readObj(string filepath);
};

inline mat4x4 mulmat4(mat4x4 a, mat4x4 b) {
	mat4x4 Result = mat4x4();
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				Result[i][j] += a[i][k] * b[k][j];
			}
		}
	}
	return Result;
}