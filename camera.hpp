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
	vec3 position, rotation, forward_vec, up_vec, right_vec;
	mat4x4 camera_matrix, viewport_matrix, projection_matrix;

	double fov, aspect_ratio;
	double sensor_size, near_clip, far_clip;
	uint32_t x_resolution, y_resolution;

	camera();

	void processMatrix(const uint16_t resx, const uint16_t resy);
};