#pragma once

#include "include.hpp"

#include "mesh.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "scene.hpp"

enum Shader {
	GOURAD,
	SUN,
	CLOUDS,
	SHIP
};

void renderPoint(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const ivec2& point, const vec3& color);
void renderLine(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const vec2& pointA, const vec2& pointB);
void renderMesh(SDL_Renderer* renderer, Scene& scene, const Mesh& mesh, const vec3& color, const Shader& type, const Texture& texture);