#include "renderer.hpp"

void renderPoint(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const ivec2& point, const vec3& color) {

	if (point.x < resx && point.x >= 0 && point.y < resy && point.y >= 0) {
		SDL_SetRenderDrawColor(renderer, int(color.x * 255.0f), int(color.y * 255.0f), int(color.z * 255.0f), 255);
		SDL_RenderDrawPoint(renderer, point.x, resy - point.y);
	}
}

void renderLine(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const vec2& pointA, const vec2& pointB) {
	int dx = abs(pointB.x - pointA.x);
	int dy = abs(pointB.y - pointA.y);
	int err = dx - dy;
	int x = pointA.x;
	int y = pointA.y;

	renderPoint(renderer, resx, resy, ivec2(x, y), vec3(1.0f));

	while (x != pointB.x || y != pointB.y) {
		renderPoint(renderer, resx, resy, ivec2(x, y), vec3(1.0f));

		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x += (pointA.x < pointB.x) ? 1 : -1;
		}
		if (e2 < dx) {
			err += dx;
			y += (pointA.y < pointB.y) ? 1 : -1;
		}
	}
}

void renderTri(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, vector<vector<float>>& Zbuffer, const vec3& vertA, const vec3& vertB, const vec3& vertC) {
	const int minX = min({ vertA.x, vertB.x, vertC.x });
	const int maxX = max({ vertA.x, vertB.x, vertC.x }) + 1.0f;
	const int minY = min({ vertA.y, vertB.y, vertC.y });
	const int maxY = max({ vertA.y, vertB.y, vertC.y }) + 1.0f;

	for (int x = minX; x < maxX; x++) {
		for (int y = minY; y < maxY; y++) {
			if (x >= 0 && x < resx && y >= 0 && y < resy) {

				const float AreaPBC = (vertB.y - vertC.y) * (x - vertC.x) + (vertC.x - vertB.x) * (y - vertC.y);
				const float AreaACP = (vertC.y - vertA.y) * (x - vertC.x) + (vertA.x - vertC.x) * (y - vertC.y);
				const float AreaABC = (vertB.y - vertC.y) * (vertA.x - vertC.x) + (vertC.x - vertB.x) * (vertA.y - vertC.y);

				const float u = AreaPBC / AreaABC;
				const float v = AreaACP / AreaABC;
				const float w = 1.0f - u - v;

				if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f && w >= 0.0f && w <= 1.0f) {
					const float Depth = u * vertA.z + v * vertB.z + w * vertC.z;
					if (Depth < Zbuffer[x][y]) {
						Zbuffer[x][y] = Depth;
						renderPoint(renderer, resx, resy, ivec2(x, y), vec3(1.0f));
					}
				}
			}
		}
	}
}
