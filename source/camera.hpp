#pragma once

#include "include.hpp"

struct Camera {
	vec3 pos, rot;
	mat4x4 camera_mat, viewport_mat, projection_mat;
	array<vec4, 6> frustum;

	double fov, aspect_ratio;
	double near_clip, far_clip;
	uint32_t res_x, res_y;


	Camera() {
		pos = vec3(0, 0, 5);
		rot = vec3(0, 0, 0);

		fov = 40.0;
		near_clip = 0.1;
		far_clip = 1000.0;
		res_x = 1800;
		res_y = 1100;

		aspect_ratio = double(res_x) / double(res_y);

		camera_mat = mat4x4();
		viewport_mat = mat4x4();
		projection_mat = mat4x4();
		frustum = array<vec4, 6>() ;
	}

	bool insideFrustum(const Vertex& v1, const Vertex& v2, const Vertex& v3) const {
		for (const auto& plane : frustum) {
			if (distancePlainToPoint(plane, v1.pos) < 0 &&
				distancePlainToPoint(plane, v2.pos) < 0 &&
				distancePlainToPoint(plane, v3.pos) < 0) {
				return false; // Triangle is outside this plane
			}
		}
		return true; // Triangle is inside the frustum
	}

	static array<vec4, 6> frustumPlanes(const mat4x4& viewProjectionMatrix) {
		array<vec4, 6> planes = {};
		// Right plane
		planes[0] = vec4(viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0],
			viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0],
			viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0],
			viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0]);

		// Left plane
		planes[1] = vec4(viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0],
			viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0],
			viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0],
			viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0]);

		// Bottom plane
		planes[2] = vec4(viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1],
			viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1],
			viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1],
			viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1]);

		// Top plane
		planes[3] = vec4(viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1],
			viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1],
			viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1],
			viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1]);

		// Near plane
		planes[4] = vec4(viewProjectionMatrix[0][2],
			viewProjectionMatrix[1][2],
			viewProjectionMatrix[2][2],
			viewProjectionMatrix[3][2]);

		// Far plane
		planes[5] = vec4(viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2],
			viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2],
			viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2],
			viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2]);

		// Normalize the planes
		for (int i = 0; i < 6; i++) {
			planes[i] = normalize(planes[i]);
		}

		return planes;
	}

	void process(const uint16_t resx, const uint16_t resy) {
		res_x = resx;
		res_y = resy;

		const mat4x4 translation = mat4x4(
			1, 0, 0, pos.x,
			0, 1, 0, pos.y,
			0, 0, 1, pos.z,
			0, 0, 0, 1
		);

		const float Yaw = rot.x * 0.017453293f;
		const float Pitch = rot.y * 0.017453293f;
		const float Roll = rot.z * 0.017453293f;

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

		camera_mat = translation * (pitchMat * yawMat * rollMat);

		aspect_ratio = float(res_x) / float(res_y);

		const float top = tan((fov * 0.0174532925) / 2.0) * near_clip;
		const float right = top * aspect_ratio;

		projection_mat = mat4x4(
			near_clip / right, 0, 0, 0,
			0, near_clip / top, 0, 0,
			0, 0, -(far_clip + near_clip) / (far_clip - near_clip), -(2.0f * far_clip * near_clip) / (far_clip - near_clip),
			0, 0, -1, 0
		);

		viewport_mat = mat4x4(
			float(res_x) / 2.0f, 0, 0, float(res_x) / 2.0f,
			0, float(res_y) / 2.0f, 0, float(res_y) / 2.0f,
			0, 0, 0.5f, 0.5f,
			0, 0, 0, 1
		);

		frustum = frustumPlanes(viewport_mat);
	}

	void move(const float& x, const float& y, const float& z, const float & sensitivity) {
		vec3 forward(0.0f, 0.0f, -1.0f);
		vec3 up(0.0f, 1.0f, 0.0f);
		vec3 right(1.0f, 0.0f, 0.0f);

		forward = rotateX(forward, rot.x * DEG_RAD);
		forward = rotateY(forward, rot.y * DEG_RAD);
		forward = rotateZ(forward, rot.z * DEG_RAD);
		forward = (forward);

		up = rotateX(up, rot.x * DEG_RAD);
		up = rotateY(up, rot.y * DEG_RAD);
		up = rotateZ(up, rot.z * DEG_RAD);
		up = normalize(up);

		right = rotateX(right, rot.x * DEG_RAD);
		right = rotateY(right, rot.y * DEG_RAD);
		right = rotateZ(right, rot.z * DEG_RAD);
		right = normalize(right);

		pos += right * x * sensitivity + up * y * sensitivity + forward * z * sensitivity;
	}

	void rotate(const float& x, const float& y, const float& z) {

	}
};