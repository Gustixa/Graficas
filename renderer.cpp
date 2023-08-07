#include "renderer.hpp"

void renderPoint(SDL_Renderer* renderer, const glm::ivec2& point) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawPoint(renderer, point.x, point.y);
}

void renderLine(SDL_Renderer* renderer, const glm::vec2& pointA, const glm::vec2& pointB) {
	int dx = abs(pointB.x - pointA.x);
	int dy = abs(pointB.y - pointA.y);
	int err = dx - dy;
	int x = pointA.x;
	int y = pointA.y;

	renderPoint(renderer, glm::ivec2(x, y));

	while (x != pointB.x || y != pointB.y) {
		renderPoint(renderer, glm::ivec2(x, y));

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