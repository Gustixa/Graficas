#include "renderer.hpp"
#include "mesh.hpp"
#include "camera.hpp"


SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

class Scene {
public:
	uint16_t RESX, RESY;
	vector<Mesh> objects;
	glm::vec3 sun;
	camera cam;
	Scene(uint16_t resx, uint16_t resy) {
		RESX = resx;
		RESY = resy;
		Zbuffer = vector<vector<float>>(RESX, vector<float>(RESY, 10000.0f));
		sun = glm::vec3(1,0,0);
		cam = camera();
	}
	vector<vector<float>> Zbuffer;
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

	float scale = 0.25;
	float visualTranslateX = int(0);
	float visualTranslateY = int(0);

	for (const Mesh& mesh : scene.objects) {
		for (const Triangle& triangle : mesh.faces) {
			glm::vec3 v1 = mesh.vertices[triangle.i1];
			glm::vec3 v2 = mesh.vertices[triangle.i2];
			glm::vec3 v3 = mesh.vertices[triangle.i3];

			double Aspect_Ratio = double(scene.RESX) / double(scene.RESY);

			int x1 = static_cast<int>(((v1.x + visualTranslateX) * scale + 1.0f) * 0.5f * scene.RESX);
			int y1 = static_cast<int>(((v1.y + visualTranslateY) * scale * Aspect_Ratio + 1.0f) * 0.5f * scene.RESY);
			int x2 = static_cast<int>(((v2.x + visualTranslateX) * scale + 1.0f) * 0.5f * scene.RESX);
			int y2 = static_cast<int>(((v2.y + visualTranslateY) * scale * Aspect_Ratio + 1.0f) * 0.5f * scene.RESY);
			int x3 = static_cast<int>(((v3.x + visualTranslateX) * scale + 1.0f) * 0.5f * scene.RESX);
			int y3 = static_cast<int>(((v3.y + visualTranslateY) * scale * Aspect_Ratio + 1.0f) * 0.5f * scene.RESY);

			renderTri(renderer, scene.RESX, scene.RESY, scene.Zbuffer, glm::vec3(x1, y1, v1.z), glm::vec3(x2, y2, v2.z), glm::vec3(x3, y3, v3.z));
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
	scene.objects.push_back(Mesh::readObj("./Logo.obj"));

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