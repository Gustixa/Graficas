#pragma once

#include <algorithm>
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <SDL.h>
#include <vector>

#include "mesh.hpp"
#include "scene.hpp"

using namespace std;
using namespace glm;

void renderPoint(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const glm::ivec2& point, const glm::vec3& color);

void renderLine(SDL_Renderer* renderer, const uint16_t resx, const uint16_t resy, const glm::vec2& pointA, const glm::vec2& pointB);

void renderShaded(SDL_Renderer* renderer, Scene& scene, const Vertex& v1, const Vertex& v2, const Vertex& v3);