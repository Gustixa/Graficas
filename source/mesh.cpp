#include "mesh.hpp"

#include "scene.hpp"

Mesh::Mesh() {
	rotation = vec3(0.0f);
	position = vec3(0.0f);
	scale    = vec3(1.0f);

	vertex_positions = vector<vec3>();
	vertex_normals = vector<vec3>();
	vertex_colors = vector<vec3>();
	vertex_uvs = vector<vec2>();
	faces = vector<Triangle>();

	output_faces = vector<Triangle>();
	output_vertices = map<size_t, Vertex>();
}

mat4 Mesh::process() const {
	const mat4 translation_matrix = mat4(
		1, 0, 0, position.x,
		0, 1, 0, position.y,
		0, 0, 1, position.z,
		0, 0, 0, 1
	);

	const float Yaw   = rotation.x * DEG_RAD;
	const float Pitch = rotation.y * DEG_RAD;
	const float Roll  = rotation.z * DEG_RAD;

	mat4 rotation_Matrix = mat4(1.0f);  // Start with an identity matrix
	rotation_Matrix = rotate(rotation_Matrix, Yaw  , vec3(0.0f, 1.0f, 0.0f));
	rotation_Matrix = rotate(rotation_Matrix, Pitch, vec3(1.0f, 0.0f, 0.0f));
	rotation_Matrix = rotate(rotation_Matrix, Roll , vec3(0.0f, 0.0f, 1.0f));

	const mat4 scale_matrix = mat4(
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1
	);

	return rotation_Matrix * scale_matrix * translation_matrix;
}

void Mesh::processVertices(const Scene& scene) {
		mat4 inv = inverse(scene.camera.camera_mat);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (inv[i][j] == -0.0f) {
					inv[i][j] = 0.0f;
				}
			}
		}

		const mat4 model_mat = process();
		const mat4 View_Matrix = mulmat4(mulmat4(mulmat4(scene.camera.viewport_mat, scene.camera.projection_mat), inv), model_mat);

		output_faces = vector<Triangle>();
		output_vertices = map<size_t, Vertex>();

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
			if (Tri.n1 != -1) vert1.normal = vertex_normals[Tri.n1];

			Vertex vert2 = Vertex();
			vert2.pos = vec3(
				vertShader2.x / vertShader2.w,
				vertShader2.y / vertShader2.w,
				vertShader2.z / vertShader2.w
			);
			Tri.u2 != -1 ? vert2.texcoord = vertex_uvs[Tri.u2] : vert2.texcoord = vec2(0);
			if (Tri.n2 != -1) vert2.normal = vertex_normals[Tri.n2];

			Vertex vert3 = Vertex();
			vert3.pos = vec3(
				vertShader3.x / vertShader3.w,
				vertShader3.y / vertShader3.w,
				vertShader3.z / vertShader3.w
			);
			Tri.u3 != -1 ? vert3.texcoord = vertex_uvs[Tri.u3] : vert3.texcoord = vec2(0);
			if (Tri.n3 != -1) vert3.normal = vertex_normals[Tri.n3];

			// View Culling
			if ((vert1.pos.x >= 0 && vert1.pos.x < scene.RESX &&
				vert1.pos.y >= 0 && vert1.pos.y < scene.RESY) ||
				(vert2.pos.x >= 0 && vert2.pos.x < scene.RESX &&
				vert2.pos.y >= 0 && vert2.pos.y < scene.RESY) ||
				(vert3.pos.x >= 0 && vert3.pos.x < scene.RESX &&
				vert3.pos.y >= 0 && vert3.pos.y < scene.RESY)
				)
			{
				output_faces.push_back(Tri);
				output_vertices[Tri.i1] = vert1;
				output_vertices[Tri.i2] = vert2;
				output_vertices[Tri.i3] = vert3;
			}
		}
}

Mesh Mesh::readObj(string filepath) {
	ifstream file(filepath, ios::in);

	Mesh mesh = Mesh();

	string line;
	while (getline(file, line)) {
		vector<string> token = Parser::splitString(line);
		if (!token.empty()) {
			if (token[0] == "v") {
				vec3 pos(
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
					stof(token[1]),
					stof(token[2])
				);
				mesh.vertex_uvs.push_back(Pos);
			}
			else if (token[0] == "vn") {
				vec3 Normal(
					stof(token[1]),
					stof(token[2]),
					stof(token[3])
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