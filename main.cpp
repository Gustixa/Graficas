#include "source/renderer.hpp"
#include "source/mesh.hpp"
#include "source/texture.hpp"
#include "source/camera.hpp"
#include "source/scene.hpp"
#include "source/include.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
array<bool, 12> input = array<bool, 12>();
bool pause = false;

// Variables
float moveSensitivity = 1000.0;
float lookSensitivity = 25.0;
const uint16_t RESX = 1920;
const uint16_t RESY = 1080;

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

	scene.cam.position = vec3(0, 4500, 0);
	scene.cam.rotation = vec3(90, 0, 0);

	// Load .obj
	scene.objects["Sun"         ] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Mercury"     ] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Venus"       ] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Earth"       ] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Moon"        ] = Mesh::readObj("./resources/moon.obj");
	scene.objects["Mars"        ] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Jupiter"     ] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Saturn"      ] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Saturn Rings"] = Mesh::readObj("./resources/rings.obj");
	scene.objects["Uranus :)"   ] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Neptune"     ] = Mesh::readObj("./resources/planet.obj");
	scene.objects["Pluto :("    ] = Mesh::readObj("./resources/moon.obj");

	// Load.png

//	scene.textures["Sun"] = loadImage("./resources/sun.png");
	
	// Position
	scene.objects["Sun"         ].position = vec3(0);
	scene.objects["Mercury"     ].position = vec3(39.00, 0, 0);
	scene.objects["Venus"       ].position = vec3(72.00, 0, 0);
	scene.objects["Earth"       ].position = vec3(100.0, 0, 0);
	scene.objects["Moon"        ].position = vec3(110.0, 0, 0);
	scene.objects["Mars"        ].position = vec3(152.0, 0, 0);
	scene.objects["Jupiter"     ].position = vec3(520.0, 0, 0);
	scene.objects["Saturn"      ].position = vec3(954.0, 0, 0);
	scene.objects["Saturn Rings"].position = vec3(954.0, 0, 0);
	scene.objects["Uranus :)"   ].position = vec3(1920., 0, 0);
	scene.objects["Neptune"     ].position = vec3(3006., 0, 0);
	scene.objects["Pluto :("    ].position = vec3(3900., 0, 0);

	// Scale
	scene.objects["Sun"         ].scale = vec3(13.910);
	scene.objects["Mercury"     ].scale = vec3(0.2240);
	scene.objects["Venus"       ].scale = vec3(0.6052);
	scene.objects["Earth"       ].scale = vec3(0.6371);
	scene.objects["Moon"        ].scale = vec3(0.1740);
	scene.objects["Mars"        ].scale = vec3(0.3390);
	scene.objects["Jupiter"     ].scale = vec3(6.9910);
	scene.objects["Saturn"      ].scale = vec3(5.8232);
	scene.objects["Saturn Rings"].scale = vec3(5.8232);
	scene.objects["Uranus :)"   ].scale = vec3(2.5362);
	scene.objects["Neptune"     ].scale = vec3(2.4622);
	scene.objects["Pluto :("    ].scale = vec3(0.0188);

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
				if (event.key.keysym.sym == SDLK_p) {
					pause = !pause;
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
	if (input[KEY_W]) scene.cam.move_camera( 0.0, -0.1,  0.0, moveSensitivity * delta_time);
	if (input[KEY_S]) scene.cam.move_camera( 0.0,  0.1,  0.0, moveSensitivity * delta_time);
	if (input[KEY_D]) scene.cam.move_camera( 0.1,  0.0,  0.0, moveSensitivity * delta_time);
	if (input[KEY_A]) scene.cam.move_camera(-0.1,  0.0,  0.0, moveSensitivity * delta_time);
	if (input[KEY_E]) scene.cam.move_camera( 0.0,  0.0,  1.0, moveSensitivity * delta_time);
	if (input[KEY_Q]) scene.cam.move_camera( 0.0,  0.0, -1.0, moveSensitivity * delta_time);
	if (input[KEY_R]) {
		scene.cam.position = vec3(0, 4500, 0);
		scene.objects["Sun"         ].position = vec3(0);
		scene.objects["Mercury"     ].position = vec3(39.00, 0, 0);
		scene.objects["Venus"       ].position = vec3(72.00, 0, 0);
		scene.objects["Earth"       ].position = vec3(100.0, 0, 0);
		scene.objects["Moon"        ].position = vec3(110.0, 0, 0);
		scene.objects["Mars"        ].position = vec3(152.0, 0, 0);
		scene.objects["Jupiter"     ].position = vec3(520.0, 0, 0);
		scene.objects["Saturn"      ].position = vec3(954.0, 0, 0);
		scene.objects["Saturn Rings"].position = vec3(954.0, 0, 0);
		scene.objects["Uranus :)"   ].position = vec3(1920., 0, 0);
		scene.objects["Neptune"     ].position = vec3(3006., 0, 0);
		scene.objects["Pluto :("    ].position = vec3(3900., 0, 0);
		scene.objects["Sun"         ].rotation = vec3(0);
		scene.objects["Mercury"     ].rotation = vec3(0);
		scene.objects["Venus"       ].rotation = vec3(0);
		scene.objects["Earth"       ].rotation = vec3(0);
		scene.objects["Moon"        ].rotation = vec3(0);
		scene.objects["Mars"        ].rotation = vec3(0);
		scene.objects["Jupiter"     ].rotation = vec3(0);
		scene.objects["Saturn"      ].rotation = vec3(0);
		scene.objects["Saturn Rings"].rotation = vec3(0);
		scene.objects["Uranus :)"   ].rotation = vec3(0);
		scene.objects["Neptune"     ].rotation = vec3(0);
		scene.objects["Pluto :("    ].rotation = vec3(0);
	}
	//if (input[KEY_DOWN])  scene.cam.rotate_camera(  0.0,  1.0, 0.0, lookSensitivity * delta_time);
	//if (input[KEY_UP])    scene.cam.rotate_camera(  0.0, -1.0, 0.0, lookSensitivity * delta_time);
	//if (input[KEY_RIGHT]) scene.cam.rotate_camera(  1.0,  0.0, 0.0, lookSensitivity * delta_time);
	//if (input[KEY_LEFT])  scene.cam.rotate_camera( -1.0,  0.0, 0.0, lookSensitivity * delta_time);

	// Scene Tick Update ----------------------------------------------------------------------------------------------------------
	scene.cam.process(scene.RESX, scene.RESY);
		// Planets
	if (!pause) {
		rotateAroundAnchor(scene.objects["Mercury"     ].position, scene.objects["Sun"].position, vec3(0, 1.0 / 0.8797 * delta_time, 0));
		rotateAroundAnchor(scene.objects["Venus"       ].position, scene.objects["Sun"].position, vec3(0, 1.0 / 2.2470 * delta_time, 0));
		rotateAroundAnchor(scene.objects["Earth"       ].position, scene.objects["Sun"].position, vec3(0, 1.0 / 3.6525 * delta_time, 0));
		rotateAroundAnchor(scene.objects["Moon"        ].position, scene.objects["Sun"].position, vec3(0, 1.0 / 3.6525 * delta_time, 0));
		rotateAroundAnchor(scene.objects["Mars"        ].position, scene.objects["Sun"].position, vec3(0, 1.0 / 6.8693 * delta_time, 0));
		rotateAroundAnchor(scene.objects["Jupiter"     ].position, scene.objects["Sun"].position, vec3(0, 1.0 / 4.3330 * delta_time, 0));
		rotateAroundAnchor(scene.objects["Saturn"      ].position, scene.objects["Sun"].position, vec3(0, 1.0 / 10.759 * delta_time, 0));
		rotateAroundAnchor(scene.objects["Saturn Rings"].position, scene.objects["Sun"].position, vec3(0, 1.0 / 10.759 * delta_time, 0));
		rotateAroundAnchor(scene.objects["Uranus :)"   ].position, scene.objects["Sun"].position, vec3(0, 1.0 / 30.687 * delta_time, 0));
		rotateAroundAnchor(scene.objects["Neptune"     ].position, scene.objects["Sun"].position, vec3(0, 1.0 / 60.190 * delta_time, 0));
		rotateAroundAnchor(scene.objects["Pluto :("    ].position, scene.objects["Sun"].position, vec3(0, 1.0 / 90.582 * delta_time, 0));
		// PLanet Rotations
		scene.objects["Sun"         ].rotation += vec3(63.84 * delta_time, 0, 0);
		scene.objects["Mercury"     ].rotation += vec3(-422.4 * delta_time, 0, 0);
		scene.objects["Venus"       ].rotation += vec3(580.8 * delta_time, 0, 0);
		scene.objects["Earth"       ].rotation += vec3(-24.00 * delta_time, 0, 0);
		scene.objects["Mars"        ].rotation += vec3(-24.39 * delta_time, 0, 0);
		scene.objects["Jupiter"     ].rotation += vec3(-10.00 * delta_time, 0, 0);
		scene.objects["Saturn"      ].rotation += vec3(-10.39 * delta_time, 0, 0);
		scene.objects["Uranus :)"   ].rotation += vec3(0, -17.14 * delta_time, 0);
		scene.objects["Neptune"     ].rotation += vec3(-16.00 * delta_time, 0, 0);
		scene.objects["Pluto :("    ].rotation += vec3(-24.00 * delta_time, 0, 0);

		scene.objects["Saturn Rings"].rotation += vec3(15.00 * delta_time, 5.0 * delta_time, 0);
		// Moons
		rotateAroundAnchor(scene.objects["Moon"].position, scene.objects["Earth"].position, vec3(0, -2.0 * delta_time, 0));
		scene.objects["Moon"].rotation += vec3(-40.0 * delta_time, 0, 0);
	}

	// Vertex Shader
	for (pair<const string, Mesh>& mesh : scene.objects) {
		mesh.second.processVertices(scene);
	}

	// Render
	renderMesh(renderer, scene, scene.objects["Sun"         ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Sun"         ]);
	renderMesh(renderer, scene, scene.objects["Mercury"     ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Mercury"     ]);
	renderMesh(renderer, scene, scene.objects["Venus"       ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Venus"       ]);
	renderMesh(renderer, scene, scene.objects["Earth"       ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Earth"       ]);
	renderMesh(renderer, scene, scene.objects["Moon"        ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Moon"        ]);
	renderMesh(renderer, scene, scene.objects["Mars"        ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Mars"        ]);
	renderMesh(renderer, scene, scene.objects["Jupiter"     ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Jupiter"     ]);
	renderMesh(renderer, scene, scene.objects["Saturn"      ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Saturn"      ]);
	renderMesh(renderer, scene, scene.objects["Saturn Rings"], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Saturn Rings"]);
	renderMesh(renderer, scene, scene.objects["Uranus :)"   ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Uranus :)"   ]);
	renderMesh(renderer, scene, scene.objects["Neptune"     ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Neptune"     ]);
	renderMesh(renderer, scene, scene.objects["Pluto :("    ], vec3(1.0, 1.0, 1.0), Shader::SUN, scene.textures["Pluto :("    ]);

	SDL_RenderPresent(renderer);
}