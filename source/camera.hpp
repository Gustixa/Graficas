#pragma once

#include "include.hpp"

enum Planes {
	Left,
	Right,
	Bottom,
	Top,
	Near,
	Far,
	Count,
	Combinations = Count * (Count - 1) / 2
};

struct Frustum {
	vec4 m_planes[Count];
	vec3 m_points[8];

	Frustum() {}
	Frustum(const mat4& projection_matrix) {
		mat4 matrix = transpose(projection_matrix);
		m_planes[Left]   = matrix[3] + matrix[0];
		m_planes[Right]  = matrix[3] - matrix[0];
		m_planes[Bottom] = matrix[3] + matrix[1];
		m_planes[Top]    = matrix[3] - matrix[1];
		m_planes[Near]   = matrix[3] + matrix[2];
		m_planes[Far]    = matrix[3] - matrix[2];

		vec3 crosses[Combinations] = {
			cross(vec3(m_planes[Left  ]), vec3(m_planes[Right ])),
			cross(vec3(m_planes[Left  ]), vec3(m_planes[Bottom])),
			cross(vec3(m_planes[Left  ]), vec3(m_planes[Top   ])),
			cross(vec3(m_planes[Left  ]), vec3(m_planes[Near  ])),
			cross(vec3(m_planes[Left  ]), vec3(m_planes[Far   ])),
			cross(vec3(m_planes[Right ]), vec3(m_planes[Bottom])),
			cross(vec3(m_planes[Right ]), vec3(m_planes[Top   ])),
			cross(vec3(m_planes[Right ]), vec3(m_planes[Near  ])),
			cross(vec3(m_planes[Right ]), vec3(m_planes[Far   ])),
			cross(vec3(m_planes[Bottom]), vec3(m_planes[Top   ])),
			cross(vec3(m_planes[Bottom]), vec3(m_planes[Near  ])),
			cross(vec3(m_planes[Bottom]), vec3(m_planes[Far   ])),
			cross(vec3(m_planes[Top   ]), vec3(m_planes[Near  ])),
			cross(vec3(m_planes[Top   ]), vec3(m_planes[Far   ])),
			cross(vec3(m_planes[Near  ]), vec3(m_planes[Far   ]))
		};

		m_points[0] = intersection<Left , Bottom, Near>(crosses);
		m_points[1] = intersection<Left , Top   , Near>(crosses);
		m_points[2] = intersection<Right, Bottom, Near>(crosses);
		m_points[3] = intersection<Right, Top   , Near>(crosses);
		m_points[4] = intersection<Left , Bottom, Far >(crosses);
		m_points[5] = intersection<Left , Top   , Far >(crosses);
		m_points[6] = intersection<Right, Bottom, Far >(crosses);
		m_points[7] = intersection<Right, Top   , Far >(crosses);
	}

	bool inside(const vec3& minp, const vec3& maxp) const {
		for (int i = 0; i < Count; i++) {
			if ((dot(m_planes[i], vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
				(dot(m_planes[i], vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
				(dot(m_planes[i], vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
				(dot(m_planes[i], vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
				(dot(m_planes[i], vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
				(dot(m_planes[i], vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
				(dot(m_planes[i], vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
				(dot(m_planes[i], vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0)) {
				return false;
			}
		}

		int out;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;

		return true;
	}

	template<Planes i, Planes j>
	struct ij2k {
		enum { k = i * (9 - i) / 2 + j - 1 };
	};

	template<Planes a, Planes b, Planes c>
	vec3 intersection(const vec3* crosses) const {
		float D = dot(vec3(m_planes[a]), crosses[ij2k<b, c>::k]);
		vec3 res = mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) *
			vec3(m_planes[a].w, m_planes[b].w, m_planes[c].w);
		return res * (-1.0f / D);
	}
};


struct Camera {
	vec3 position, rotation;
	mat4 camera_mat, viewport_mat, projection_mat;
	Frustum view_frustum;

	float fov, near_clip, far_clip;

	Camera() {
		position = vec3(0, 0, 0);
		rotation = vec3(0, 0, 0);

		fov = 50.0;
		near_clip = 0.1;
		far_clip = 1000.0;

		camera_mat = mat4(1);
		viewport_mat = mat4(1);
		projection_mat = mat4(1);

		view_frustum = Frustum(camera_mat * projection_mat);
	}

	void process(const uint16_t resx, const uint16_t resy) {
		const mat4 translation_matrix = mat4(
			1, 0, 0, position.x,
			0, 1, 0, position.y,
			0, 0, 1, position.z,
			0, 0, 0, 1
		);

		const float Yaw   = rotation.x * DEG_RAD;
		const float Pitch = rotation.y * DEG_RAD;
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

		view_frustum = Frustum(projection_mat * camera_mat);
	}

	void move_camera(const float& x, const float& y, const float& z, const float & sensitivity) {
		mat4 rotation_Matrix = mat4(1.0f);
		rotation_Matrix = rotate(rotation_Matrix, x * DEG_RAD, vec3(0.0f, 1.0f, 0.0f));
		rotation_Matrix = rotate(rotation_Matrix, y * DEG_RAD, vec3(1.0f, 0.0f, 0.0f));
		rotation_Matrix = rotate(rotation_Matrix, z * DEG_RAD, vec3(0.0f, 0.0f, 1.0f));

		vec3 forward = vec3(rotation_Matrix * vec4(0.0f, 0.0f, -1.0f, 0.0f));
		vec3 up      = vec3(rotation_Matrix * vec4(0.0f, 1.0f,  0.0f, 0.0f));
		vec3 right   = vec3(rotation_Matrix * vec4(1.0f, 0.0f,  0.0f, 0.0f));

		position += right * x * sensitivity + up * y * sensitivity + forward * z * sensitivity;
	}

	void rotate_camera(const float& x, const float& y, const float& z, const float& sensitivity) {
		if (rotation.y + y > 89.0f)  rotation += vec3(x,  89.0f, x) * sensitivity;
		if (rotation.y + y < -89.0f) rotation += vec3(x, -89.0f, x) * sensitivity;
		rotation += vec3(x, y, x) * sensitivity;
	}
};