#pragma once

#include "include.hpp"

struct Camera {
	vec3 position, rotation;
	mat4 camera_mat, viewport_mat, projection_mat;

	float fov, near_clip, far_clip;

	Camera() {
		position = vec3(0, 0, 0);
		rotation = vec3(0, 0, 0);

		fov = 80.0;
		near_clip = 0.1;
		far_clip = 1000.0;

		camera_mat = mat4(1);
		viewport_mat = mat4(1);
		projection_mat = mat4(1);
	}

	void process(const uint16_t resx, const uint16_t resy) {
		const mat4 translation_matrix = mat4(
			1, 0, 0, position.x,
			0, 1, 0, position.y,
			0, 0, 1, position.z,
			0, 0, 0, 1
		);

		const float Pitch = rotation.x * DEG_RAD;
		const float Yaw   = rotation.y * DEG_RAD;
		const float Roll  = rotation.z * DEG_RAD;

		mat4 rotation_Matrix = mat4(1.0f);
		rotation_Matrix = rotate(rotation_Matrix, Yaw  , vec3(0.0f, 1.0f, 0.0f));
		rotation_Matrix = rotate(rotation_Matrix, Pitch, vec3(1.0f, 0.0f, 0.0f));
		rotation_Matrix = rotate(rotation_Matrix, Roll , vec3(0.0f, 0.0f, 1.0f));

		camera_mat = rotation_Matrix * translation_matrix;

		float aspect_ratio = float(resx) / float(resy);

		const float top = tan((fov * DEG_RAD) / 2.0) * near_clip;
		const float right = top * aspect_ratio;

		projection_mat = perspective(fov * DEG_RAD, aspect_ratio, near_clip, far_clip);

		viewport_mat = mat4(
			float(resx) / 2.0f, 0, 0, float(resx) / 2.0f,
			0, float(resy) / 2.0f, 0, float(resy) / 2.0f,
			0, 0, 0.5f, 0.5f,
			0, 0, 0, 1
		);
	}

	void move_camera(const float& x, const float& y, const float& z, const float & sensitivity) {
		mat4 rotation_Matrix = mat4(1.0f);
		rotation_Matrix = rotate(rotation_Matrix, (rotation.y + y) * DEG_RAD, vec3(0.0f, 1.0f, 0.0f));
		rotation_Matrix = rotate(rotation_Matrix, (rotation.x + x) * DEG_RAD, vec3(1.0f, 0.0f, 0.0f));
		rotation_Matrix = rotate(rotation_Matrix, (rotation.z + z) * DEG_RAD, vec3(0.0f, 0.0f, 1.0f));

		vec3 z_vec = vec3(rotation_Matrix * vec4(0.0f, 0.0f, -1.0f, 0.0f));
		vec3 y_vec = vec3(rotation_Matrix * vec4(0.0f, 1.0f,  0.0f, 0.0f));
		vec3 x_vec = vec3(rotation_Matrix * vec4(1.0f, 0.0f,  0.0f, 0.0f));

		position += (x_vec * x + y_vec * y + z_vec * z) * sensitivity;
		position.x = glm::clamp(position.x, -5000.0f, 5000.0f);
		position.y = glm::clamp(position.y,     0.0f, 5000.0f);
		position.z = glm::clamp(position.z, -5000.0f, 5000.0f);
	}

	void rotate_camera(const float& x, const float& y, const float& z, const float& sensitivity) {
		rotation += vec3(x, y, z) * sensitivity;
	}

	void view(const vec3& eye_pos, const vec3& object) {
		mat4 rotation_Matrix = mat4(1.0f);
		rotation_Matrix = rotate(rotation_Matrix, (rotation.y) * DEG_RAD, vec3(0.0f, 1.0f, 0.0f));
		rotation_Matrix = rotate(rotation_Matrix, (rotation.x) * DEG_RAD, vec3(1.0f, 0.0f, 0.0f));
		rotation_Matrix = rotate(rotation_Matrix, (rotation.z) * DEG_RAD, vec3(0.0f, 0.0f, 1.0f));

		vec3 y_vec = vec3(rotation_Matrix * vec4(0.0f, 1.0f, 0.0f, 0.0f));

		const mat4 viewMatrix = lookAt(eye_pos, object, y_vec);
		rotation.x = degrees(asin(-viewMatrix[1][2]));
		rotation.y = degrees(atan2(viewMatrix[0][2], viewMatrix[2][2]));
		rotation.z = degrees(atan2(viewMatrix[1][0], viewMatrix[1][1]));
	}
};