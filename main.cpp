#include "renderer.hpp"
#include "mesh.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

class Scene {
public:
	uint16_t RESX, RESY;
	vector<Mesh> objects;
	Scene(uint16_t resx, uint16_t resy) {
		RESX = resx;
		RESY = resy;
	}
};

void init(const uint16_t RESX, const uint16_t RESY) {
	SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RESX, RESY, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void render(Scene scene) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


	for (const Mesh& mesh : scene.objects) {
		for (const Triangle& triangle : mesh.faces) {
			glm::vec3 v1 = mesh.vertices[triangle.i1];
			glm::vec3 v2 = mesh.vertices[triangle.i2];
			glm::vec3 v3 = mesh.vertices[triangle.i3];

			double Aspect_Ratio = double(scene.RESX) / double(scene.RESY);

			int x1 = static_cast<int>((v1.x + 1.0f) * 0.5f * scene.RESX);
			int y1 = static_cast<int>((v1.y * Aspect_Ratio + 1.0f) * 0.5f * scene.RESY);
			int x2 = static_cast<int>((v2.x + 1.0f) * 0.5f * scene.RESX);
			int y2 = static_cast<int>((v2.y * Aspect_Ratio + 1.0f) * 0.5f * scene.RESY);
			int x3 = static_cast<int>((v3.x + 1.0f) * 0.5f * scene.RESX);
			int y3 = static_cast<int>((v3.y * Aspect_Ratio + 1.0f) * 0.5f * scene.RESY);

			if (x1 < scene.RESX && x1 >= 0 && x2 < scene.RESX && x2 >= 0 && y1 < scene.RESY && y1 >= 0 && y2 < scene.RESY && y2 >= 0)
				renderLine(renderer, glm::vec2(x1, y1), glm::vec2(x2, y2));

			if (x3 < scene.RESX && x3 >= 0 && x2 < scene.RESX && x2 >= 0 && y3 < scene.RESY && y3 >= 0 && y2 < scene.RESY && y2 >= 0)
				renderLine(renderer, glm::vec2(x2, y2), glm::vec2(x3, y3));

			if (x1 < scene.RESX && x1 >= 0 && x3 < scene.RESX && x3 >= 0 && y1 < scene.RESY && y1 >= 0 && y3 < scene.RESY && y3 >= 0)
				renderLine(renderer, glm::vec2(x3, y3), glm::vec2(x1, y1));
		}
	}

	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
	const uint16_t RESX = 1920;
	const uint16_t RESY = 1080;

	init(RESX, RESY);
	bool running = true;
	
	Scene scene = Scene(RESX, RESY);
	scene.objects.push_back(Mesh::readObj("./nave.obj"));

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}
		render(scene);
	}

	return 0;
}