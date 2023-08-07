#include "mesh.hpp"

Mesh Mesh::readObj(string filepath) {
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
				mesh.vertices.push_back(pos);
			}
			else if (token[0] == "f") {
				vector<string> v1 = Parser::splitStringD(token[1], "/");
				vector<string> v2 = Parser::splitStringD(token[2], "/");
				vector<string> v3 = Parser::splitStringD(token[3], "/");

				Triangle triangle = {
					stoull(v1[0]) - 1,
					stoull(v2[0]) - 1,
					stoull(v3[0]) - 1
				};

				mesh.faces.push_back(triangle);
			}
		}
	}
	return mesh;
}