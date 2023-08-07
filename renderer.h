#pragma once

#include <glm/glm.hpp>
#include <SDL.h>

void renderPoint(SDL_Renderer* renderer, const glm::ivec2& point);

void renderLine(SDL_Renderer* renderer, const glm::vec2& pointA, const glm::vec2& pointB);