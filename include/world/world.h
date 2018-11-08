#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include <glm/glm.hpp>
#include "camera.h"
#include "misc/fft_noise_generator.h"
#include "world_const.h"
#include "texture/itexture.h"
#include "models/chunk_model.h"

class World {
public:
	World(uint32_t seed);
	~World();

	
	void generate_world(const std::shared_ptr<ITexture>& chunk_texture);
	float height_at(float x, float z) const;

	std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>> get_height_map() const;
	std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> get_chunks() const;
	private:
	std::shared_ptr<FFTNoiseGenerator> m_generator;
	std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>> m_height_map;

	std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> m_chunks;

	uint32_t m_seed;
};
