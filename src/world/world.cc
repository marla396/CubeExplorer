#include <utility>

#include "world/world.h"

World::World(uint32_t seed) : m_all_blocks_initialized(false), m_seed(seed) {

	m_sun = std::make_shared<Light>( glm::vec3 { WORLD_SIZE * CHUNK_SIZE * 1.2f, WORLD_MAX_HEIGHT * 1.5f, WORLD_SIZE * CHUNK_SIZE * 1.2f}, glm::vec3 { 1.0f, 1.0f, 1.0f });

	m_generator = std::make_shared<FFTNoiseGenerator>(seed, 0.005f, 2.0f, 1.0f);
}

World::~World() {

}

void World::update(float time) {

	if (!m_all_blocks_initialized) {

		bool initialized = true;

		for (auto& chunk : *m_chunks) {
			if (!chunk->is_initialized()) {
				chunk->gpu_init();
				initialized = false;
			}
		}

		m_all_blocks_initialized = initialized;
	}

	float r = WORLD_CENTER.x * 1.2f;

	glm::vec3 new_pos = { WORLD_CENTER.x + r * std::cos(time/10.0f), WORLD_CENTER.y + r * std::sin(time/10.0f), WORLD_CENTER.z + r * std::cos(time / 10.0f) };

	m_sun->set_position(new_pos);
}

float World::height_at(float x, float z) const {
	if (x >= 0.0f && z >= 0.0f && x + 0.5f < static_cast<float>(WORLD_SIZE * CHUNK_SIZE) && z + 0.5f < static_cast<float>(WORLD_SIZE * CHUNK_SIZE)) {
		return std::floor(m_height_map->noise[static_cast<int>(x + 0.5f)][static_cast<int>(z + 0.5f)]);
	}
	return 0.0f;
}

void World::generate_world(const std::shared_ptr<ITexture>& chunk_texture) {

	m_height_map = m_generator->generate_height_map<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>(1.0f, static_cast<float>(WORLD_MAX_HEIGHT));
	m_chunks = std::make_shared<std::vector<std::shared_ptr<ChunkModel>>>();

	int n_workers = WORLD_GENERATOR_THREADS;
	for (int i = 0; i < n_workers; i++) {
		std::thread t(&World::generate_world_part, this, n_workers, i, chunk_texture);
		t.detach();
	}
}

void World::lock_chunks() {
	m_generator_mutex.lock();
}

void World::unlock_chunks() {
	m_generator_mutex.unlock();
}

std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>> World::get_height_map() const {
	return m_height_map;
}

std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> World::get_chunks() const {
	return m_chunks;
}

std::shared_ptr<Light> World::get_sun() const {
	return m_sun;
}

void World::generate_world_part(int n_workers, int id, const std::shared_ptr<ITexture>& chunk_texture) {

	int x_start = id * (WORLD_SIZE / n_workers);
	int x_end = (id + 1) * (WORLD_SIZE / n_workers);

	for (int x = x_start; x < x_end; x++) {
		for (int z = 0; z < WORLD_SIZE; z++) {
			for (int y = 0; y < WORLD_MAX_HEIGHT / CHUNK_SIZE; y++) {

				glm::vec3 chunk_pos = { x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE };

				BlockMap block_map = {};

				block_map.set_global_height(y * CHUNK_SIZE);

				int n_blocks = 0;

				for (int cx = 0; cx < CHUNK_SIZE; cx++) {
					for (int cz = 0; cz < CHUNK_SIZE; cz++) {
						int height = static_cast<int>(m_height_map->noise[x * CHUNK_SIZE + cx][z * CHUNK_SIZE + cz]) - y * CHUNK_SIZE;

						if (height < 2 * CHUNK_SIZE) {
							for (int cy = 0; cy < height && cy < CHUNK_SIZE; cy++) {
								block_map.insert_block(cx, cy, cz, height < CHUNK_SIZE + 1);
								if (height != CHUNK_SIZE + 1)
									n_blocks++;
							}
						}
					}
				}


				if (n_blocks < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) { //is not solid?	
					std::shared_ptr<ChunkModel> chunk = std::make_shared<ChunkModel>(chunk_texture, chunk_pos, block_map);
					add_chunk(chunk);
				}
			}
		}
	}
}

void World::add_chunk(const std::shared_ptr<ChunkModel>& chunk) {
	m_generator_mutex.lock();

	m_chunks->push_back(chunk);	
	m_all_blocks_initialized = false;

	m_generator_mutex.unlock();

}