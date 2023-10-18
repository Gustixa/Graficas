#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

vector<vector<vec3>> loadImage(const char* imagePath) {
	int width, height, channels;

	// Load the image using stb_image.h
	unsigned char* image_data = stbi_load(imagePath, &width, &height, &channels, 3);

	if (!image_data) {
		cerr << "Error loading image." << endl;
		exit(1);
	}

	vector<vector<vec3>> result;

	for (int y = 0; y < height; ++y) {
		vector<vec3> row;
		for (int x = 0; x < width; ++x) {
			int offset = (y * width + x) * 3;

			vec3 pixel(
				image_data[offset] / 255.0f,
				image_data[offset + 1] / 255.0f,
				image_data[offset + 2] / 255.0f
			);

			row.push_back(pixel);
		}
		result.push_back(row);
	}
	return result;

	stbi_image_free(image_data);
}