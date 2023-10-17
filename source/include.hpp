#pragma once

#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <windows.h>
#include <stdexcept>
#include <iostream>
#include <optional>
#include <direct.h>
#include <stdint.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <cerrno>
#include <vector>
#include <math.h>
#include <ctime>
#include <array>
#include <map>

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#undef min
#undef max
#undef clamp
#undef near
#undef far

#define PI          float(3.14159265f)
#define TWO_PI      float(6.28318531f)
#define INVERTED_PI float(0.31830989f)
#define DEG_RAD     float(0.01745329f)


inline mat4x4 mulmat4(mat4x4 a, mat4x4 b) {
	mat4x4 Result = mat4x4();
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				Result[i][j] += a[i][k] * b[k][j];
			}
		}
	}
	return Result;
}

inline vec3 triangleNormal(const vec3& v1, const vec3& v2, const vec3& v3) {
	return normalize(cross(v3 - v1, v2 - v1));
}

inline void rotateAroundAnchor(vec3& position, const vec3& anchor, float angle) {
	mat4 translateToAnchor = translate(mat4(1.0f), anchor);
	mat4 rotatation = rotate(angle, vec3(0.0f, 0.0f, 1.0f));
	mat4 translateBack = translate(mat4(1.0f), -anchor);
	mat4 transformation = translateBack * rotatation * translateToAnchor;
	vec4 transformedPosition = transformation * vec4(position, 1.0f);
	position = vec3(transformedPosition);
}

inline float distancePlainToPoint(const vec4& plane, const vec3& point) {
	float numerator = abs(plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w);
	float denominator = sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
	return numerator / denominator;
}