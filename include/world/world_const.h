#pragma once

#include <glm/glm.hpp>

constexpr int CHUNK_SIZE = 16;
constexpr int WORLD_SIZE = 4;
constexpr int WORLD_MAX_HEIGHT = 16;
constexpr float WORLD_WATER_HEIGHT = 5.5f;
constexpr float WORLD_BEACH_HEIGHT = WORLD_WATER_HEIGHT + 1.0f;
constexpr size_t WATER_FFT_DIMENSION = 256;
constexpr size_t WATER_FFT_DIMENSION_LOG2 = 8;


class ChunkVector3Hash {
public:
	size_t operator()(const glm::ivec3& v) const {
		//Courtesy of http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf

		constexpr size_t p1 = 73856093;
		constexpr size_t p2 = 19349663;
		constexpr size_t p3 = 83492791;

		return (v.x * p1) ^ (v.y * p2) ^ (v.z * p3);
	}
};