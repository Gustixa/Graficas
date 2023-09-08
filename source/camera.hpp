#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace glm;

class camera {
public:
	vec3 pos, rot;
	mat4x4 camera_mat, viewport_mat, projection_mat;

	double fov, aspect_ratio;
	double near_clip, far_clip;
	uint32_t res_x, res_y;

	camera();

	void processMatrix(const uint16_t resx, const uint16_t resy);
};