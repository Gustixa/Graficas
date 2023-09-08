#pragma once

#include "mesh.hpp"
#include "camera.hpp"

class Scene {
public:
	uint16_t RESX, RESY;
	vector<Mesh> objects;
	glm::vec3 sun;
	camera cam;
	Scene(uint16_t resx, uint16_t resy) {
		RESX = resx;
		RESY = resy;
		Zbuffer = vector<vector<float>>(RESX, vector<float>(RESY, 10000.0f));
		sun = normalize(vec3(1.0, 0.5, 1.0));
		cam = camera();
	}
	vector<vector<float>> Zbuffer;
};