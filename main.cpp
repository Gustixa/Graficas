#include "source/renderer.hpp"
#include "source/mesh.hpp"
#include "source/camera.hpp"
#include "source/scene.hpp"


SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

void init(const uint16_t RESX, const uint16_t RESY) {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RESX, RESY, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void render(Scene scene) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	scene.cam.processMatrix(scene.RESX, scene.RESY);

	for (Mesh& mesh : scene.objects) {
		mesh.processMatrix();
		mesh.processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat);

		for (const Triangle& tri : mesh.faces) {
			const Vertex& v1 = mesh.vertex_output[tri.i1];
			const Vertex& v2 = mesh.vertex_output[tri.i2];
			const Vertex& v3 = mesh.vertex_output[tri.i3];
			renderShaded(renderer, scene, v1, v2, v3);
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
	scene.objects.push_back(Mesh::readObj("./resources/Moon.obj"));

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