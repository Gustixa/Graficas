#include "camera.hpp"

camera::camera() {
	position = vec3(0, 0, 5);
	rotation = vec3(0, 0, 0);
	right_vec = vec3(1, 0, 0);
	up_vec = vec3(0, 1, 0);
	forward_vec = vec3(0, 0, 1);

	fov = 40.0;
	near_clip = 0.1;
	far_clip = 1000.0;
	res_x = 1800;
	res_y = 1100;

	aspect_ratio = float(res_x) / float(res_y);

	camera_mat = mat4x4();
	viewport_mat = mat4x4();
	projection_mat = mat4x4();
}

void camera::processMatrix(const uint16_t resx, const uint16_t resy) {
	res_x = resx;
	res_y = resy;

	const mat4x4 translation = mat4x4(
		1, 0, 0, position.x,
		0, 1, 0, position.y,
		0, 0, 1, position.z,
		0, 0, 0, 1
	);

	const float Yaw = rotation.x * 0.0174532925;
	const float Pitch = rotation.y * 0.0174532925;
	const float Roll = rotation.z * 0.0174532925;

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
		0, 0, 0.5, 0.5,
		0, 0, 0, 1
	);
}