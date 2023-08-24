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

	scene.cam.processMatrix(scene.RESX, scene.RESY);

	//for (Mesh& mesh : scene.objects) {
	//	mesh.processMatrix();
	//	mesh.processVertices(scene.cam.camera_matrix, scene.cam.projection_matrix, scene.cam.viewport_matrix);
	//	for (const Triangle& tri : mesh.faces) {
	//		const Vertex& v1 = mesh.vertex_output[tri.i1];
	//		const Vertex& v2 = mesh.vertex_output[tri.i2];
	//		const Vertex& v3 = mesh.vertex_output[tri.i3];
	//		const int minX = min({ v1.Pos.x, v2.Pos.x, v3.Pos.x });
	//		const int maxX = max({ v1.Pos.x, v2.Pos.x, v3.Pos.x }) + 1;
	//		const int minY = min({ v1.Pos.y, v2.Pos.y, v3.Pos.y });
	//		const int maxY = max({ v1.Pos.y, v2.Pos.y, v3.Pos.y }) + 1;
	//		for (int x = minX; x < maxX; x++) {
	//			for (int y = minY; y < maxY; y++) {
	//				if (x >= 0 && x < scene.RESX && y >= 0 && y < scene.RESY) {
	//					
	//					const float AreaPBC = (v2.Pos.y - v3.Pos.y) * (x - v3.Pos.x) + (v3.Pos.x - v2.Pos.x) * (y - v3.Pos.y);
	//					const float AreaACP = (v3.Pos.y - v1.Pos.y) * (x - v3.Pos.x) + (v1.Pos.x - v3.Pos.x) * (y - v3.Pos.y);
	//					const float AreaABC = (v2.Pos.y - v3.Pos.y) * (v1.Pos.x - v3.Pos.x) + (v3.Pos.x - v2.Pos.x) * (v1.Pos.y - v3.Pos.y);

	//					const float u = AreaPBC / AreaABC;
	//					const float v = AreaACP / AreaABC;
	//					const float w = 1.0 - u - v;

	//					if (u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0 && w >= 0.0 && w <= 1.0) {
	//						const float Depth = u * v1.Pos.z + v * v2.Pos.z + w * v3.Pos.z;
	//						if (Depth < scene.Zbuffer[x][y]) {
	//							scene.Zbuffer[x][y] = Depth;
	//							renderPoint(renderer, scene.RESX, scene.RESY, vec2(x, y), v1.Color * u + v2.Color * v + v3.Color * w);
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	
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
	cout << "Rendered";

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