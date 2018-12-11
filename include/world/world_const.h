#ifndef WORLD_CONST_H
#define WORLD_CONST_H
#include <glm/glm.hpp>

constexpr int CHUNK_SIZE = 16;
constexpr int WORLD_SIZE = 2;
extern int WORLD_MAX_HEIGHT;
extern float WORLD_WATER_HEIGHT;
extern float WORLD_BEACH_HEIGHT;
constexpr size_t WATER_FFT_DIMENSION = 256;
constexpr size_t WATER_FFT_DIMENSION_LOG2 = 8;
extern int WATER_QUAD_DIMENSION;

constexpr int WORLD_SHEEP_AMOUNT = 0;

constexpr int WORLD_GENERATOR_THREADS = 1;

constexpr int SHADOW_CASCADES = 6;

const glm::vec3 WORLD_CENTER = glm::vec3 { static_cast<float>(WORLD_SIZE * CHUNK_SIZE), static_cast<float>(WORLD_MAX_HEIGHT), static_cast<float>(WORLD_SIZE * CHUNK_SIZE) } * 0.5f;

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
#endif
