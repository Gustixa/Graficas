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
		sun = normalize(vec3(1,0.5,0));
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

	for (Mesh& mesh : scene.objects) {
		mesh.processMatrix();
		mesh.processVertices(scene.cam.camera_mat, scene.cam.projection_mat, scene.cam.viewport_mat);

		vector<float> Z_Positions;
		for (const Vertex& vert : mesh.vertex_output) {
			Z_Positions.push_back(vert.pos.z);
		}
		const float minZ = *min_element(Z_Positions.begin(), Z_Positions.end());
		const float maxZ = *max_element(Z_Positions.begin(), Z_Positions.end());

		int i = 0;
		for (const Triangle& tri : mesh.faces) {
			const Vertex& v1 = mesh.vertex_output[tri.i1];
			const Vertex& v2 = mesh.vertex_output[tri.i2];
			const Vertex& v3 = mesh.vertex_output[tri.i3];
			const int minX = min({ v1.pos.x, v2.pos.x, v3.pos.x });
			const int maxX = max({ v1.pos.x, v2.pos.x, v3.pos.x }) + 1;
			const int minY = min({ v1.pos.y, v2.pos.y, v3.pos.y });
			const int maxY = max({ v1.pos.y, v2.pos.y, v3.pos.y }) + 1;
			for (int x = minX; x < maxX; x++) {
				for (int y = minY; y < maxY; y++) {
					if (x >= 0 && x < scene.RESX && y >= 0 && y < scene.RESY) {
						
						const float AreaPBC = (v2.pos.y - v3.pos.y) * (x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (y - v3.pos.y);
						const float AreaACP = (v3.pos.y - v1.pos.y) * (x - v3.pos.x) + (v1.pos.x - v3.pos.x) * (y - v3.pos.y);
						const float AreaABC = (v2.pos.y - v3.pos.y) * (v1.pos.x - v3.pos.x) + (v3.pos.x - v2.pos.x) * (v1.pos.y - v3.pos.y);

						const float u = AreaPBC / AreaABC;
						const float v = AreaACP / AreaABC;
						const float w = 1.0 - u - v;

						if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f && w >= 0.0f && w <= 1.0f) {
							const float Depth = u * v1.pos.z + v * v2.pos.z + w * v3.pos.z;
							vec3 Color = v1.col * u + v2.col * v + v3.col * w;
							float Sun_Intensity = dot((u * v1.normal + v * v2.normal + w * v3.normal), scene.sun);
							if (Sun_Intensity < 0.0f) {
								Sun_Intensity = 0.0f;
							}
							Color *= Sun_Intensity;
							if (Depth < scene.Zbuffer[x][y]) {
								scene.Zbuffer[x][y] = Depth;
								renderPoint(renderer, scene.RESX, scene.RESY, vec2(x, y), Color);
							}
						}
					}
				}
			}
			cout << "Rendered:  " << float(i) / float(mesh.faces.size()) * 100.0 << "%" << endl;
			i++;
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
	scene.objects.push_back(Mesh::readObj("./mesh.obj"));

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