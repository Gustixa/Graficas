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

void render(Scene& scene) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	scene.cam.rot += vec3(1.5, 0, 0);
	scene.cam.processMatrix(scene.RESX, scene.RESY);
	scene.objects["Saturn Rings"].rot += vec3(1, 1, 1);
	scene.sun += vec3(2.5, 2.5, 0);
	scene.Zbuffer = vector<vector<float>>(scene.RESX, vector<float>(scene.RESY, 10000.0f));
	vec3 sun = scene.getSun();
	vec3 camera = scene.getCamera();

	vector<Vertex> vertices = scene.objects["Moon"].processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat, scene.objects["Moon"].processMatrix());
	for (const Triangle& tri : scene.objects["Moon"].faces) {
		const Vertex& v1 = vertices[tri.i1];
		const Vertex& v2 = vertices[tri.i2];
		const Vertex& v3 = vertices[tri.i3];
		renderShaded(renderer, scene, sun, v1, v2, v3);
	}

	vertices = scene.objects["Saturn"].processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat, scene.objects["Saturn"].processMatrix());
	for (const Triangle& tri : scene.objects["Saturn"].faces) {
		const Vertex& v1 = vertices[tri.i1];
		const Vertex& v2 = vertices[tri.i2];
		const Vertex& v3 = vertices[tri.i3];
		renderGas(renderer, scene, sun, camera, v1, v2, v3);
	}

	vertices = scene.objects["Saturn Rings"].processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat, scene.objects["Saturn Rings"].processMatrix());
	for (const Triangle& tri : scene.objects["Saturn Rings"].faces) {
		const Vertex& v1 = vertices[tri.i1];
		const Vertex& v2 = vertices[tri.i2];
		const Vertex& v3 = vertices[tri.i3];
		renderShaded(renderer, scene, sun, v1, v2, v3);
	}

	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
	const uint16_t RESX = 1920;
	const uint16_t RESY = 1080;

	init(RESX, RESY);
	bool running = true;
	
	Scene scene = Scene(RESX, RESY);

	scene.cam.pos = vec3(0,0,10);
	scene.cam.rot = vec3(0,-15,0);

	scene.objects["Moon"] = Mesh::readObj("./resources/Moon.obj");
	scene.objects["Saturn"] = Mesh::readObj("./resources/Planet.obj");
	scene.objects["Saturn Rings"] = Mesh::readObj("./resources/Rings.obj");

	scene.objects["Moon"].pos = vec3(3, 0, 0);
	scene.objects["Saturn"].pos = vec3(-2.5, 0, 0);
	scene.objects["Saturn Rings"].pos = vec3(-2.5, 0, 0);
	scene.objects["Saturn Rings"].rot = vec3(0, 0, 15);

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