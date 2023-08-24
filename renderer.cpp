#include "renderer.hpp"

void renderPoint(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const glm::ivec2& point) {

	if (point.x < resx && point.x >= 0 && point.y < resy && point.y >= 0) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawPoint(renderer, point.x, resy - point.y);
	}
}

void renderLine(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const glm::vec2& pointA, const glm::vec2& pointB) {
	int dx = abs(pointB.x - pointA.x);
	int dy = abs(pointB.y - pointA.y);
	int err = dx - dy;
	int x = pointA.x;
	int y = pointA.y;

	renderPoint(renderer, resx, resy, glm::ivec2(x, y));

	while (x != pointB.x || y != pointB.y) {
		renderPoint(renderer, resx, resy, glm::ivec2(x, y));

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

void renderTri(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, vector<vector<float>>& Zbuffer, const glm::vec3& vertA, const glm::vec3& vertB, const glm::vec3& vertC) {
	const int minX = min({ vertA.x, vertB.x, vertC.x });
	const int maxX = max({ vertA.x, vertB.x, vertC.x }) + 1;
	const int minY = min({ vertA.y, vertB.y, vertC.y });
	const int maxY = max({ vertA.y, vertB.y, vertC.y }) + 1;

	for (int x = minX; x < maxX; x++) {
		for (int y = minY; y < maxY; y++) {
			if (x >= 0 && x < resx && y >= 0 && y < resy) {

				const double AreaPBC = (vertB.y - vertC.y) * (x - vertC.x) + (vertC.x - vertB.x) * (y - vertC.y);
				const double AreaACP = (vertC.y - vertA.y) * (x - vertC.x) + (vertA.x - vertC.x) * (y - vertC.y);
				const double AreaABC = (vertB.y - vertC.y) * (vertA.x - vertC.x) + (vertC.x - vertB.x) * (vertA.y - vertC.y);

				const double u = AreaPBC / AreaABC;
				const double v = AreaACP / AreaABC;
				const double w = 1.0 - u - v;

				if (u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0 && w >= 0.0 && w <= 1.0) {
					const double Depth = u * vertA.z + v * vertB.z + w * vertC.z;
					if (Depth < Zbuffer[x][y]) {
						Zbuffer[x][y] = Depth;
						renderPoint(renderer, resx, resy, glm::ivec2(x, y));
					}
				}
			}
		}
	}
}
