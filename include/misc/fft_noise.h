#pragma once

#include <array>

template<size_t W, size_t D>
struct FFTNoise {
	std::array<std::array<float, D>, W> noise;
	float average;
	float min;
	float max;

	uint8_t* to_texture_buffer() const {

		uint8_t *buffer = new uint8_t[W * D * 4];

		for (size_t x = 0; x < W; x++) {
			for (size_t z = 0; z < D; z++) {
				uint8_t gray_scale = static_cast<uint8_t>(255.0f * (noise[x][z] - min) / (max - min));

				buffer[4 * (z + x * D)] = gray_scale;
				buffer[4 * (z + x * D) + 1] = gray_scale;
				buffer[4 * (z + x * D) + 2] = gray_scale;
				buffer[4 * (z + x * D) + 3] = 255;
			}
		}

		return buffer;
	}
};