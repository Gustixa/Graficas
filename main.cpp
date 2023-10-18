#include "source/renderer.hpp"
#include "source/mesh.hpp"
#include "source/camera.hpp"
#include "source/scene.hpp"
#include "source/include.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
array<bool, 11> input = array<bool, 11>();

// Variables
float moveSensitivity = 1.0;
float lookSensitivity = 25.0;
const uint16_t RESX = 600;
const uint16_t RESY = 400;

double update_time = 0.5;
double window_time = 0;
double delta_time = 0;
double run_time = 0;
clock_t last_time = 0;
clock_t current_time = 0;

void init(const uint16_t RESX, const uint16_t RESY);
void render(Scene& Scene);

int main(int argc, char* argv[]) {
	init(RESX, RESY);
	bool running = true;

	Scene scene = Scene(RESX, RESY);

	scene.cam.position = vec3(0, 0, 10);
	scene.cam.rotation = vec3(0, 0, 0);

	scene.objects["Sun"]          = Mesh::readObj("./resources/planet.obj");
	scene.objects["Earth"]        = Mesh::readObj("./resources/planet.obj");
	scene.objects["Moon"]         = Mesh::readObj("./resources/moon.obj");
	scene.objects["Saturn"]       = Mesh::readObj("./resources/planet.obj");
	scene.objects["Saturn Rings"] = Mesh::readObj("./resources/rings.obj");

	scene.objects["Sun"].position          = vec3(0, 0, 0);
	scene.objects["Earth"].position        = vec3(5, 0, 0);
	scene.objects["Moon"].position         = vec3(7, 0, 0);
	scene.objects["Saturn"].position       = vec3(0, 0, 0);
	scene.objects["Saturn Rings"].position = vec3(0, 0, 0);

	scene.objects["Earth"].scale  = vec3(0.5);
	scene.objects["Moon"].scale   = vec3(0.2);

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				input[getKey(event.key.keysym.sym)] = true;
			}
			else if (event.type == SDL_KEYUP) {
				input[getKey(event.key.keysym.sym)] = false;
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					SDL_Quit();
					return 0;
				}
			}
			else if (event.type == SDL_MOUSEWHEEL) {
				if (event.wheel.preciseY > 0)
					moveSensitivity *= 1.1;
				if (event.wheel.preciseY < 0) {
					moveSensitivity /= 1.1;
				}
			}
		}
		render(scene);
	}
	return 0;
}

void init(const uint16_t RESX, const uint16_t RESY) {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RESX, RESY, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void render(Scene& scene) {
	current_time = clock();
	delta_time = float(current_time - last_time) / CLOCKS_PER_SEC;
	last_time = current_time;
	run_time += delta_time;
	window_time += delta_time;

	if (window_time > update_time) {
		window_time -= update_time;
		double fps = 1.0 / delta_time;
		stringstream stream;
		stream << fixed << setprecision(2) << fps;
		string strFps = stream.str();
		SDL_SetWindowTitle(window, ("Rasterizer | " + strFps + " FPS").c_str());
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	scene.Zbuffer = vector<vector<float>>(scene.RESX, vector<float>(scene.RESY, 10000.0f));


	// User Input Update
	if (input[KEY_W]) scene.cam.move_camera( 0.0,  0.0,  1.0, moveSensitivity * delta_time);
	if (input[KEY_S]) scene.cam.move_camera( 0.0,  0.0, -1.0, moveSensitivity * delta_time);
	if (input[KEY_D]) scene.cam.move_camera( 1.0,  0.0,  0.0, moveSensitivity * delta_time);
	if (input[KEY_A]) scene.cam.move_camera(-1.0,  0.0,  0.0, moveSensitivity * delta_time);
	if (input[KEY_E]) scene.cam.move_camera( 0.0,  1.0,  0.0, moveSensitivity * delta_time);
	if (input[KEY_Q]) scene.cam.move_camera( 0.0, -1.0,  0.0, moveSensitivity * delta_time);
	if (input[KEY_DOWN])  scene.cam.rotate_camera(  0.0,  1.0, 0.0, lookSensitivity * delta_time);
	if (input[KEY_UP])    scene.cam.rotate_camera(  0.0, -1.0, 0.0, lookSensitivity * delta_time);
	if (input[KEY_RIGHT]) scene.cam.rotate_camera(  1.0,  0.0, 0.0, lookSensitivity * delta_time);
	if (input[KEY_LEFT])  scene.cam.rotate_camera( -1.0,  0.0, 0.0, lookSensitivity * delta_time);
	// Scene Update
	scene.cam.process(scene.RESX, scene.RESY);
	rotateAroundAnchor(scene.objects["Earth"].position, scene.objects["Sun"].position, vec3(0, 0.2 * delta_time, 0));
	rotateAroundAnchor(scene.objects["Moon"].position, scene.objects["Earth"].position, vec3(0, -2.0 * delta_time, 0));
	scene.objects["Moon"].rotation += vec3(-40.0 * delta_time, 0, 0);

	// Scene Render
	scene.objects["Sun"].processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat, scene.objects["Sun"].process());
	renderMesh(renderer, scene, scene.cam, scene.objects["Sun"]);

	scene.objects["Earth"].processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat, scene.objects["Earth"].process());
	renderMesh(renderer, scene, scene.cam, scene.objects["Earth"]);
	
	scene.objects["Moon"].processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat, scene.objects["Moon"].process());
	renderMesh(renderer, scene, scene.cam, scene.objects["Moon"]);

	SDL_RenderPresent(renderer);
}