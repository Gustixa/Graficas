#pragma once

#include "include.hpp"

#include "mesh.hpp"
#include "camera.hpp"
#include "scene.hpp"

void renderPoint(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const glm::ivec2& point, const glm::vec3& color);
void renderLine(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const glm::vec2& pointA, const glm::vec2& pointB);
void renderMesh(SDL_Renderer* renderer, Scene& scene, const Camera& camera, const Mesh& mesh);