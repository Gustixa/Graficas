#pragma once

#include "mesh.hpp"
#include "camera.hpp"

struct Scene {
	uint16_t RESX, RESY;
	map<string, Mesh> objects;
	vec3 sun;
	Camera cam;
	Scene(uint16_t resx, uint16_t resy) {
		RESX = resx;
		RESY = resy;
		Zbuffer = vector<vector<float>>(RESX, vector<float>(RESY, 10000.0f));
		sun = vec3(0, 0, 0);
		cam = Camera();
	}
	vector<vector<float>> Zbuffer;

	vec3 getSun() {
		float yaw =   sun.x * DEG_RAD;
		float pitch = sun.y * DEG_RAD;
		float roll =  sun.z * DEG_RAD;

		mat4 rotationMatrix(1.0f);
		rotationMatrix = rotate(rotationMatrix, pitch, vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = rotate(rotationMatrix, yaw,   vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = rotate(rotationMatrix, roll,  vec3(0.0f, 0.0f, 1.0f));

		return normalize(vec3(rotationMatrix * vec4(0.0f, 0.0f, -1.0f, 0.0f)));
	}

	vec3 getCamera() {
		float yaw =   cam.rot.x * DEG_RAD;
		float pitch = cam.rot.y * DEG_RAD;
		float roll =  cam.rot.z * DEG_RAD;

		mat4 rotationMatrix(1.0f);
		rotationMatrix = rotate(rotationMatrix, yaw,   vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = rotate(rotationMatrix, pitch, vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = rotate(rotationMatrix, roll,  vec3(0.0f, 0.0f, 1.0f));

		return vec3(rotationMatrix * vec4(0.0f, 0.0f, 1.0f, 0.0f));
	}
};