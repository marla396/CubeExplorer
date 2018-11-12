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
#include "texture/mtexture.h"
#include "models/chunk_model.h"
#include "entities/player.h"
#include "light.h"

class World {
public:
	World(uint32_t seed);
	~World();

	void update(float time);
	void generate_world(const std::shared_ptr<ITexture>& chunk_texture);
	void lock_chunks();
	void unlock_chunks();
	void set_seed(uint32_t seed);

	float height_at(float x, float z) const;

	std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>> get_height_map() const;
	std::shared_ptr<MTexture<uint8_t>> get_height_map_texture() const;
	std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> get_chunks() const;

	std::shared_ptr<Light> get_sun() const;
	std::shared_ptr<Player> get_player() const;
private:
	void generate_world_part(int n_workers, int id, const std::shared_ptr<ITexture>& chunk_texture);
	void add_chunk(const std::shared_ptr<ChunkModel>& chunk);

	std::shared_ptr<FFTNoiseGenerator> m_generator;
	std::mutex m_generator_mutex;
	bool m_all_blocks_initialized;

	std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>> m_height_map;

	std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> m_chunks;
	std::shared_ptr<Player> m_player;
	std::shared_ptr<Light> m_sun;
	std::shared_ptr<MTexture<uint8_t>> m_height_map_texture;

	uint32_t m_seed;
};
