#pragma once

#include "include.hpp"

struct Scene;
struct Parser;
struct Vertex;
struct Triangle;
struct Mesh;

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
		i1 = -1; i2 = -1; i3 = -1;
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

struct Mesh {
	vec3 rotation, position, scale;

	vector<vec3> vertex_positions;
	vector<vec3> vertex_colors;
	vector<vec3> vertex_normals;
	vector<vec2> vertex_uvs;

	vector<Triangle> faces;

	vector<Triangle> output_faces;
	map<size_t, Vertex> output_vertices;

	Mesh();

	mat4 process() const;
	void processVertices(const Scene& scene);
	static Mesh readObj(string filepath);
};