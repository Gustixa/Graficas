#include "renderer.hpp"

void renderPoint(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const glm::ivec2& point, const glm::vec3& color) {

	if (point.x < resx && point.x >= 0 && point.y < resy && point.y >= 0) {
		SDL_SetRenderDrawColor(renderer, int(color.x * 255.0f), int(color.y * 255.0f), int(color.z * 255.0f), 255);
		SDL_RenderDrawPoint(renderer, point.x, resy - point.y);
	}
}

void renderLine(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const glm::vec2& pointA, const glm::vec2& pointB) {
	int dx = abs(pointB.x - pointA.x);
	int dy = abs(pointB.y - pointA.y);
	int err = dx - dy;
	int x = pointA.x;
	int y = pointA.y;

	renderPoint(renderer, resx, resy, glm::ivec2(x, y), glm::vec3(1.0f));

	while (x != pointB.x || y != pointB.y) {
		renderPoint(renderer, resx, resy, glm::ivec2(x, y), glm::vec3(1.0f));

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

void renderShaded(SDL_Renderer* renderer, Scene& scene, const vec3& sun, const Vertex& v1, const Vertex& v2, const Vertex& v3) {
	const int minX = std::min({ v1.pos.x, v2.pos.x, v3.pos.x });
	const int maxX = std::max({ v1.pos.x, v2.pos.x, v3.pos.x }) + 1;
	const int minY = std::min({ v1.pos.y, v2.pos.y, v3.pos.y });
	const int maxY = std::max({ v1.pos.y, v2.pos.y, v3.pos.y }) + 1;
	for (int x = minX; x < maxX; x++) {
		for (int y = minY; y < maxY; y++) {
			if (x >= 0 && x < scene.RESX && y >= 0 && y < scene.RESY) {

				const float AreaPBC = (v2.pos.y - v3.pos.y) * (x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (y - v3.pos.y);
				const float AreaACP = (v3.pos.y - v1.pos.y) * (x - v3.pos.x) + (v1.pos.x - v3.pos.x) * (y - v3.pos.y);
				const float AreaABC = (v2.pos.y - v3.pos.y) * (v1.pos.x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (v1.pos.y - v3.pos.y);

				const float u = AreaPBC / AreaABC;
				const float v = AreaACP / AreaABC;
				const float w = 1.0 - u - v;

				if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f && w >= 0.0f && w <= 1.0f) {
					const float Depth = u * v1.pos.z + v * v2.pos.z + w * v3.pos.z;
					vec3 Color = v1.col * u + v2.col * v + v3.col * w;
					float Sun_Intensity = dot((u * v1.normal + v * v2.normal + w * v3.normal), sun);
					Sun_Intensity = clamp(Sun_Intensity, 0.0f, 1.0f);
					Color *= Sun_Intensity;
					if (Depth < scene.Zbuffer[x][y]) {
						scene.Zbuffer[x][y] = Depth;
						renderPoint(renderer, scene.RESX, scene.RESY, vec2(x, y), Color);
					}
				}
			}
		}
	}
}
