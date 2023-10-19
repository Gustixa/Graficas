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

void renderMesh(SDL_Renderer* renderer, Scene& scene, const Mesh& mesh, const vec3& color, const Shader& type, const Texture& texture) {
	for (const Triangle& tri : mesh.output_faces) {
		const Vertex& v1 = mesh.output_vertices.at(tri.i1);
		const Vertex& v2 = mesh.output_vertices.at(tri.i2);
		const Vertex& v3 = mesh.output_vertices.at(tri.i3);
		const float minX = std::min({ v1.pos.x, v2.pos.x, v3.pos.x });
		const float maxX = std::max({ v1.pos.x, v2.pos.x, v3.pos.x }) + 1.0f;
		const float minY = std::min({ v1.pos.y, v2.pos.y, v3.pos.y });
		const float maxY = std::max({ v1.pos.y, v2.pos.y, v3.pos.y }) + 1.0f;
		for (int x = minX; x < maxX; x++) {
			for (int y = minY; y < maxY; y++) {
				if (x >= 0 && x < scene.RESX && y >= 0 && y < scene.RESY) {
					const float AreaPBC = (v2.pos.y - v3.pos.y) * (x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (y - v3.pos.y);
					const float AreaACP = (v3.pos.y - v1.pos.y) * (x - v3.pos.x) + (v1.pos.x - v3.pos.x) * (y - v3.pos.y);
					const float AreaABC = (v2.pos.y - v3.pos.y) * (v1.pos.x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (v1.pos.y - v3.pos.y);

					const float u = AreaPBC / AreaABC;
					const float v = AreaACP / AreaABC;
					const float w = 1.0f - u - v;

					if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f && w >= 0.0f && w <= 1.0f) {
						const float Depth = u * v1.pos.z + v * v2.pos.z + w * v3.pos.z;
						switch (type) {
							case SUN:
								if (Depth < scene.Zbuffer[x][y]) {
									scene.Zbuffer[x][y] = Depth;

									const vec2 uv = vec2(
										u * v1.texcoord.x + v * v2.texcoord.x + w * v3.texcoord.x,
										u * v1.texcoord.y + v * v2.texcoord.y + w * v3.texcoord.y
									);
									renderPoint(renderer, scene.RESX, scene.RESY, vec2(x, y), texture.getColor(uv));
								}
							case PLANET:
								if (Depth < scene.Zbuffer[x][y]) {
									scene.Zbuffer[x][y] = Depth;

									const vec2 uv = vec2(
										u * v1.texcoord.x + v * v2.texcoord.x + w * v3.texcoord.x,
										u * v1.texcoord.y + v * v2.texcoord.y + w * v3.texcoord.y
									);
									vec3 Color = texture.getColor(uv);
									vec3 Normal = normalize(u * v1.normal + v * v2.normal + w * v3.normal);
									vec3 Sun = normalize(-(u * v1.pos + v * v2.pos + w * v3.pos));
									float Sun_Intensity = pow(dot(Normal, Sun), 2);
									Sun_Intensity = glm::clamp(Sun_Intensity, 0.0f, 1.0f);
									Color *= Sun_Intensity;
									Color += vec3(0.05f); // Ambient Light
									Color = clamp(Color, 0.0f, 1.0f);
									renderPoint(renderer, scene.RESX, scene.RESY, vec2(x, y), Color);
								}
							case CLOUDS:
								if (Depth < scene.Zbuffer[x][y]) {
									const vec2 uv = vec2(
										u * v1.texcoord.x + v * v2.texcoord.x + w * v3.texcoord.x,
										u * v1.texcoord.y + v * v2.texcoord.y + w * v3.texcoord.y
									);
									vec3 Color = texture.getColor(uv);
									if (Color.x > 0.05f && Color.y > 0.05f && Color.z > 0.05f) {
										scene.Zbuffer[x][y] = Depth;
										vec3 Normal = normalize(u * v1.normal + v * v2.normal + w * v3.normal);
										vec3 Sun = normalize(-(u * v1.pos + v * v2.pos + w * v3.pos));
										float Sun_Intensity = pow(dot(Normal, Sun), 2);
										Sun_Intensity = glm::clamp(Sun_Intensity, 0.0f, 1.0f);
										Color *= Sun_Intensity;
										Color = clamp(Color, 0.0f, 1.0f);
										renderPoint(renderer, scene.RESX, scene.RESY, vec2(x, y), Color);
									}
								}
							case SHIP:
								if (Depth < scene.Zbuffer[x][y]) {
									vec3 Color = vec3(1.0);
									scene.Zbuffer[x][y] = Depth;
									vec3 Normal = normalize(u * v1.normal + v * v2.normal + w * v3.normal);
									vec3 Sun = normalize(-(u * v1.pos + v * v2.pos + w * v3.pos));
									float Sun_Intensity = pow(dot(Normal, Sun), 2);
									Sun_Intensity = glm::clamp(Sun_Intensity, 0.0f, 1.0f);
									Color *= Sun_Intensity;
									Color = clamp(Color, 0.0f, 1.0f);
									renderPoint(renderer, scene.RESX, scene.RESY, vec2(x, y), Color);
								}
						}
					}
				}
			}
		}
	}
}