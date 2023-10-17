#include "source/renderer.hpp"
#include "source/mesh.hpp"
#include "source/camera.hpp"
#include "source/scene.hpp"
#include "source/include.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
array<bool, 348> input = array<bool, 348>();

float moveSensitivity = 0.05;

void init(const uint16_t RESX, const uint16_t RESY) {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RESX, RESY, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void render(Scene& scene) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	scene.Zbuffer = vector<vector<float>>(scene.RESX, vector<float>(scene.RESY, 10000.0f));
	vec3 sun = scene.getSun();

	// User Input Update
	if (input[SDLK_w]) scene.cam.move(0.0, 0.0,  1.0, moveSensitivity);
	if (input[SDLK_s]) scene.cam.move(0.0, 0.0, -1.0, moveSensitivity);
	if (input[SDLK_a]) scene.cam.move( 1.0, 0.0, 0.0, moveSensitivity);
	if (input[SDLK_d]) scene.cam.move(-1.0, 0.0, 0.0, moveSensitivity);
	if (input[SDLK_e]) scene.cam.move(0.0,  1.0, 0.0, moveSensitivity);
	if (input[SDLK_q]) scene.cam.move(0.0, -1.0, 0.0, moveSensitivity);
	// Scene Update
	scene.cam.process(scene.RESX, scene.RESY);
	scene.objects["Saturn Rings"].rot += vec3(0.5, 0, 0);

	// Scene Render

	scene.objects["Earth"].processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat, scene.objects["Earth"].process());
	renderMesh(renderer, scene, sun, scene.cam, scene.objects["Earth"]);
	
	scene.objects["Moon"].processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat, scene.objects["Moon"].process());
	renderMesh(renderer, scene, sun, scene.cam, scene.objects["Moon"]);

	//scene.objects["Saturn"].processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat, scene.objects["Saturn"].process());
	//renderGas(renderer, scene, sun, scene.cam, scene.objects["Saturn"]);

	//scene.objects["Saturn Rings"].processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat, scene.objects["Saturn Rings"].process());
	//renderGas(renderer, scene, sun, scene.cam, scene.objects["Saturn Rings"]);

	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
	const uint16_t RESX = 600;
	const uint16_t RESY = 400;

	init(RESX, RESY);
	bool running = true;

	Scene scene = Scene(RESX, RESY);

	scene.cam.pos = vec3(0, 0, 10);
	scene.cam.rot = vec3(0, -15, 0);

	scene.objects["Sun"] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Earth"] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Moon"] = Mesh::readObj("./resources/moon.obj");
	scene.objects["Saturn"] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Saturn Rings"] = Mesh::readObj("./resources/rings.obj");

	scene.objects["Sun"].pos = vec3(0, 0, 0);
	scene.objects["Earth"].pos = vec3(0, 0, 0);
	scene.objects["Moon"].pos = vec3(2, 0, 0);
	scene.objects["Moon"].scale = vec3(0.2, 0.2, 0.2);
	scene.objects["Saturn"].pos = vec3(0, 0, 0);
	scene.objects["Saturn Rings"].pos = vec3(0, 0, 0);

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				input[event.key.keysym.sym] = true;
			}
			else if (event.type == SDL_KEYUP) {
				input[event.key.keysym.sym] = false;
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					SDL_Quit();
				}
			}
		}
		render(scene);
	}
	return 0;
}