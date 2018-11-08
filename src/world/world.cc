#include <utility>

#include "world/world.h"

World::World(uint32_t seed) : m_seed(seed) {

	m_generator = std::make_shared<FFTNoiseGenerator>(seed, 0.005f, 2.0f, 1.0f);
}

World::~World() {

}

float World::height_at(float x, float z) const {
	if (x >= 0.0f && z >= 0.0f && x + 0.5f < static_cast<float>(WORLD_SIZE * CHUNK_SIZE) && z + 0.5f < static_cast<float>(WORLD_SIZE * CHUNK_SIZE)) {
		return std::floor(m_height_map->noise[static_cast<int>(x + 0.5f)][static_cast<int>(z + 0.5f)]);
	}
	return 0.0f;
}

void World::generate_world(const std::shared_ptr<ITexture>& chunk_texture) {

	m_height_map = m_generator->generate_height_map<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>(0.0f, static_cast<float>(WORLD_MAX_HEIGHT));
	m_chunks = std::make_shared<std::vector<std::shared_ptr<ChunkModel>>>();

	for (int x = 0; x < WORLD_SIZE; x++) {
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


				if (n_blocks < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE){ //is not solid?	
					std::shared_ptr<ChunkModel> chunk = std::make_shared<ChunkModel>(chunk_texture, chunk_pos, block_map);
					m_chunks->push_back(chunk);
				}
			}
		}
	}
}

std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>> World::get_height_map() const {
	return m_height_map;
}

std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> World::get_chunks() const {
	return m_chunks;
}
