#pragma once

#include <glm/glm.hpp>
#include <SDL.h>

#include <vector>
using namespace std;
using namespace glm;

void renderPoint(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const glm::ivec2& point, const glm::ivec3& color);

void renderLine(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const glm::vec2& pointA, const glm::vec2& pointB);

void renderTri(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy,vector<vector<float>>& Zbuffer, const glm::vec3& vertA, const glm::vec3& vertB, const glm::vec3& vertC);