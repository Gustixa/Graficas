#pragma once

#include "texture.hpp"
#include "mesh.hpp"
#include "camera.hpp"

struct Scene {
	uint16_t RESX, RESY;
	map<string, Mesh> objects;
	map<string, Texture> textures;
	Camera camera;
	Scene(uint16_t resx, uint16_t resy) {
		RESX = resx;
		RESY = resy;
		Zbuffer = vector<vector<float>>(RESX, vector<float>(RESY, 10000.0f));
		camera = Camera();
	}
	vector<vector<float>> Zbuffer;

	vec3 getCamera() {
		float yaw =   camera.rotation.x * DEG_RAD;
		float pitch = camera.rotation.y * DEG_RAD;
		float roll =  camera.rotation.z * DEG_RAD;

		mat4 rotationMatrix(1.0f);
		rotationMatrix = rotate(rotationMatrix, yaw,   vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = rotate(rotationMatrix, pitch, vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = rotate(rotationMatrix, roll,  vec3(0.0f, 0.0f, 1.0f));

		return vec3(rotationMatrix * vec4(0.0f, 0.0f, 1.0f, 0.0f));
	}
};