#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>

#include <glm/glm.hpp>
#include "camera.h"
#include "misc/fft_noise_generator.h"
#include "world_const.h"
#include "texture/itexture.h"
#include "models/chunk_model.h"
#include "light.h"

class World {
public:
	World(uint32_t seed);
	~World();

	void update(float time);
	void generate_world(const std::shared_ptr<ITexture>& chunk_texture);
	void lock_chunks();
	void unlock_chunks();

	float height_at(float x, float z) const;

	std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>> get_height_map() const;
	std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> get_chunks() const;

	std::shared_ptr<Light> get_sun() const;
private:
	void generate_world_part(int n_workers, int id, const std::shared_ptr<ITexture>& chunk_texture);
	void add_chunk(const std::shared_ptr<ChunkModel>& chunk);

	std::shared_ptr<FFTNoiseGenerator> m_generator;
	std::mutex m_generator_mutex;
	bool m_all_blocks_initialized;

	std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>> m_height_map;

	std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> m_chunks;
	std::shared_ptr<Light> m_sun;

	uint32_t m_seed;
};
