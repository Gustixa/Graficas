#pragma once

#include "mesh.hpp"
#include "camera.hpp"

class Scene {
public:
	uint16_t RESX, RESY;
	map<string, Mesh> objects;
	vec3 sun;
	camera cam;
	Scene(uint16_t resx, uint16_t resy) {
		RESX = resx;
		RESY = resy;
		Zbuffer = vector<vector<float>>(RESX, vector<float>(RESY, 10000.0f));
		sun = vec3(220, 25, 0);
		cam = camera();
	}
	vector<vector<float>> Zbuffer;

	vec3 getSun() {
		float yaw = radians(sun.x);     // Rotation around the Y-axis
		float pitch = radians(sun.y);   // Rotation around the X-axis
		float roll = radians(sun.z);     // Rotation around the Z-axis

		mat4 rotationMatrix(1.0f);
		rotationMatrix = rotate(rotationMatrix, yaw, vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = rotate(rotationMatrix, pitch, vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = rotate(rotationMatrix, roll, vec3(0.0f, 0.0f, 1.0f));

		return vec3(rotationMatrix * vec4(0.0f, 0.0f, -1.0f, 0.0f));
	}
};

