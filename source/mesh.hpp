#pragma once

#include"include.hpp"

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
	vec3 rot, pos, scale;

	vector<vec3> vertex_positions;
	vector<vec3> vertex_colors;
	vector<vec3> vertex_normals;
	vector<vec2> vertex_uvs;

	vector<Triangle> faces;
	vector<Vertex> output;

	Mesh() {
		rot = vec3(0.0f);
		pos = vec3(0.0f);
		scale = vec3(1.0f);

		vertex_positions = vector<glm::vec3>();
		vertex_normals = vector<vec3>();
		vertex_colors = vector<vec3>();
		vertex_uvs = vector<vec2>();
		faces = vector<Triangle>();
		output = vector<Vertex>();
	}

	mat4x4 process() const {
		const mat4x4 translation = mat4x4(
			1, 0, 0, pos.x,
			0, 1, 0, pos.y,
			0, 0, 1, pos.z,
			0, 0, 0, 1
		);

		const double Yaw = rot.x * 0.0174532925;
		const double Pitch = rot.y * 0.0174532925;
		const double Roll = rot.z * 0.0174532925;

		const mat4x4 yawMat = mat4x4(
			cos(Yaw), 0, sin(Yaw), 0,
			0, 1, 0, 0,
			-sin(Yaw), 0, cos(Yaw), 0,
			0, 0, 0, 1
		);

		const mat4x4 pitchMat = mat4x4(
			1, 0, 0, 0,
			0, cos(Pitch), -sin(Pitch), 0,
			0, sin(Pitch), cos(Pitch), 0,
			0, 0, 0, 1
		);

		const mat4x4 rollMat = mat4x4(
			cos(Roll), -sin(Roll), 0, 0,
			sin(Roll), cos(Roll), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);

		const mat4x4 scaleMat = mat4x4(
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1
		);

		return (pitchMat * yawMat * rollMat) * scaleMat * translation;
	}

	void processVertices(const mat4x4& cam_mat, const mat4x4& proj_mat, const mat4x4& view_mat, const mat4x4& model_mat) {
		mat4x4 inverse = glm::inverse(cam_mat);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (inverse[i][j] == -0.0f) {
					inverse[i][j] = 0.0f;
				}
			}
		}

		const mat4x4 View_Matrix = mulmat4(mulmat4(mulmat4(view_mat, proj_mat), inverse), model_mat);

		output = vector<Vertex>(vertex_positions.size(), Vertex());

		for (const Triangle& Tri : faces) {
			const vec4 vertShader1 = vec4(vertex_positions[Tri.i1], 1.0f) * View_Matrix;
			const vec4 vertShader2 = vec4(vertex_positions[Tri.i2], 1.0f) * View_Matrix;
			const vec4 vertShader3 = vec4(vertex_positions[Tri.i3], 1.0f) * View_Matrix;

			Vertex vert1 = Vertex();
			vert1.pos = vec3(
				vertShader1.x / vertShader1.w,
				vertShader1.y / vertShader1.w,
				vertShader1.z / vertShader1.w
			);
			Tri.u1 != -1 ? vert1.texcoord = vertex_uvs[Tri.u1] : vert1.texcoord = vec2(0);
			Tri.n1 != -1 ? vert1.normal = vertex_normals[Tri.n1] : vert1.normal = vec3(0);
			vertex_colors.size() == vertex_positions.size() ? vert1.col = vertex_colors[Tri.i1] : vert1.col = vec3(1.0);
			output[Tri.i1] = vert1;

			Vertex vert2 = Vertex();
			vert2.pos = vec3(
				vertShader2.x / vertShader2.w,
				vertShader2.y / vertShader2.w,
				vertShader2.z / vertShader2.w
			);
			Tri.u2 != -1 ? vert2.texcoord = vertex_uvs[Tri.u2] : vert2.texcoord = vec2(0);
			Tri.n2 != -1 ? vert2.normal = vertex_normals[Tri.n2] : vert2.normal = vec3(0);
			vertex_colors.size() == vertex_positions.size() ? vert2.col = vertex_colors[Tri.i2] : vert2.col = vec3(1.0);
			output[Tri.i2] = vert2;

			Vertex vert3 = Vertex();
			vert3.pos = vec3(
				vertShader3.x / vertShader3.w,
				vertShader3.y / vertShader3.w,
				vertShader3.z / vertShader3.w
			);
			Tri.u3 != -1 ? vert3.texcoord = vertex_uvs[Tri.u3] : vert3.texcoord = vec2(0);
			Tri.n3 != -1 ? vert3.normal = vertex_normals[Tri.n3] : vert3.normal = vec3(0);
			vertex_colors.size() == vertex_positions.size() ? vert3.col = vertex_colors[Tri.i3] : vert3.col = vec3(1.0);
			output[Tri.i3] = vert3;
		}
	}

	static Mesh readObj(string filepath) {
		ifstream file(filepath, ios::in);

		Mesh mesh = Mesh();

		string line;
		while (getline(file, line)) {
			vector<string> token = Parser::splitString(line);
			if (!token.empty()) {
				if (token[0] == "v") {
					glm::vec3 pos(
						stof(token[1]),
						stof(token[2]),
						stof(token[3])
					);
					mesh.vertex_positions.push_back(pos);
					if (token.size() > 6) {
						vec3 Color = vec3(
							stof(token[4]),
							stof(token[5]),
							stof(token[6])
						);
						mesh.vertex_colors.push_back(Color);
					}
				}
				else if (token[0] == "vt") {
					vec2 Pos(
						stod(token[1]),
						stod(token[2])
					);
					mesh.vertex_uvs.push_back(Pos);
				}
				else if (token[0] == "vn") {
					vec3 Normal(
						stod(token[1]),
						stod(token[2]),
						stod(token[3])
					);
					mesh.vertex_normals.push_back(Normal);
				}
				else if (token[0] == "f") {
					vector<string> v1 = Parser::splitStringD(token[1], "/");
					vector<string> v2 = Parser::splitStringD(token[2], "/");
					vector<string> v3 = Parser::splitStringD(token[3], "/");

					Triangle triangle = Triangle();
					triangle.i1 = stoull(v1[0]) - 1;
					triangle.i2 = stoull(v2[0]) - 1;
					triangle.i3 = stoull(v3[0]) - 1;

					if (v1.size() == 3) {
						if (!v1[1].empty()) {
							triangle.u1 = stoull(v1[1]) - 1;
							triangle.u2 = stoull(v2[1]) - 1;
							triangle.u3 = stoull(v3[1]) - 1;
						}

						if (!v1[2].empty()) {
							triangle.n1 = stoull(v1[2]) - 1;
							triangle.n2 = stoull(v2[2]) - 1;
							triangle.n3 = stoull(v3[2]) - 1;
						}
					}

					mesh.faces.push_back(triangle);
				}
			}
		}
		return mesh;
	}
};