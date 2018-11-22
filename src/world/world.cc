#include <utility>

#include "world/world.h"
#include "models/tree_model.h"

World::World(uint32_t seed) : m_all_blocks_initialized(false), m_seed(seed) {

	m_sun = std::make_shared<Light>( glm::vec3 { WORLD_SIZE * CHUNK_SIZE * 1.2f, WORLD_MAX_HEIGHT * 1.5f, WORLD_SIZE * CHUNK_SIZE * 1.2f}, glm::vec3 { 1.0f, 1.0f, 1.0f });

	m_generator = std::make_shared<FFTNoiseGenerator>(seed, 0.005f, 2.0f, 1.0f);

	m_chunks = std::make_shared<std::vector<std::shared_ptr<ChunkModel>>>();
	m_entities = std::make_shared<std::vector<std::shared_ptr<ChunkModel>>>();
	m_water_models = std::make_shared<std::vector<std::shared_ptr<WaterModel>>>();
}

World::~World() {

}

void World::update(float time, Camera& camera) {

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

	//time = -(50 * PI) / 4;
	time = 0.0f;

	float r = WORLD_CENTER.x * 1.2f;

	glm::vec3 new_pos = { WORLD_CENTER.x + r * std::cos(time/5.0f + PI/4), WORLD_CENTER.y + r * std::sin(time/5.0f + PI/4), WORLD_CENTER.z + r * std::cos(time / 5.0f + PI/4) };

	m_sun->set_position(new_pos);

	m_sun->m_player_position = { camera.get_position().x + Light::LIGHT_HIGH_DISTANCE * std::cos(time / 5.0f + PI / 2), camera.get_position().y + Light::LIGHT_HIGH_DISTANCE * std::sin(time / 5.0f + PI / 2), camera.get_position().z + Light::LIGHT_HIGH_DISTANCE * std::cos(time / 5.0f + PI / 2) };
}

void World::clear_world() {
	m_chunks->clear();
	m_entities->clear();
	m_water_models->clear();
}

float World::height_at(float x, float z) const {
	if (x >= 0.0f && z >= 0.0f && x + 0.5f < static_cast<float>(WORLD_SIZE * CHUNK_SIZE) && z + 0.5f < static_cast<float>(WORLD_SIZE * CHUNK_SIZE)) {
		return std::floor(m_height_map->noise[static_cast<int>(x + 0.5f)][static_cast<int>(z + 0.5f)]);
	}
	return 0.0f;
}

void World::generate_world(const std::shared_ptr<ITexture>& chunk_texture) {

	m_height_map = m_generator->generate_height_map<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>(1.0f, static_cast<float>(WORLD_MAX_HEIGHT));

	if (m_height_map_texture != nullptr)
		m_height_map_texture->destroy();

	auto* buffer = m_height_map->to_texture_buffer();

	m_player = std::make_shared<Player>(chunk_texture);
	m_height_map_texture = std::make_shared<MTexture<uint8_t>>(WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE, buffer);

	delete[] buffer;


	for (int x = 0; x < WATER_QUAD_DIMENSION; x++) {
		for (int y = 0; y < WATER_QUAD_DIMENSION; y++) {
			glm::vec2 pos = {
				static_cast<float>(x) / static_cast<float>(WATER_QUAD_DIMENSION),
				static_cast<float>(y) / static_cast<float>(WATER_QUAD_DIMENSION)
			};

			float world_size = static_cast<float>(WORLD_SIZE * CHUNK_SIZE);
			auto water_model = std::make_shared<WaterModel>(pos);
			water_model->set_position({ -0.5f + pos.x * world_size, WORLD_WATER_HEIGHT, -0.5f + pos.y * world_size });
			water_model->set_scale({ world_size / static_cast<float>(WATER_QUAD_DIMENSION), 1.0f, world_size / static_cast<float>(WATER_QUAD_DIMENSION) });

			m_water_models->push_back(water_model);
		}
	}

	m_entities->push_back(std::dynamic_pointer_cast<PlayerModel>(m_player->get_model()));

	int n_workers = WORLD_GENERATOR_THREADS;
	for (int i = 0; i < n_workers; i++) {
		std::thread t(&World::generate_world_part, this, n_workers, i, chunk_texture);
		t.detach();
	}


}

void World::lock() {
	m_generator_mutex.lock();
}

void World::unlock() {
	m_generator_mutex.unlock();
}

void World::set_seed(uint32_t seed) {
	m_seed = seed;
}

std::shared_ptr<FFTNoise<WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE>> World::get_height_map() const {
	return m_height_map;
}

std::shared_ptr<MTexture<uint8_t>> World::get_height_map_texture() const {
	return m_height_map_texture;
}

std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> World::get_chunks() const {
	return m_chunks;
}

std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> World::get_entities() const {
	return m_entities;
}

std::shared_ptr<std::vector<std::shared_ptr<WaterModel>>> World::get_water_models() const {
	return m_water_models;
}

std::shared_ptr<Light> World::get_sun() const {
	return m_sun;
}

std::shared_ptr<Player> World::get_player() const {
	return m_player;
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

						//if (height < 2 * CHUNK_SIZE) {
							for (int cy = 0; cy < height && cy < CHUNK_SIZE; cy++) {
								block_map.insert_block(cx, cy, cz, height < CHUNK_SIZE + 1);
								if (height != CHUNK_SIZE + 1)
									n_blocks++;
							}
						//}
					}
				}


				//if (n_blocks < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) { //is not solid?	
					std::shared_ptr<ChunkModel> chunk = std::make_shared<ChunkModel>(chunk_texture, chunk_pos, block_map);
					add_chunk(chunk);

				//}
			}
		}
	}

	/*if (id == 0) {
		for (int x = 0; x < CHUNK_SIZE * WORLD_SIZE; x += CHUNK_SIZE) {
			for (int z = 0; z < CHUNK_SIZE * WORLD_SIZE; z += CHUNK_SIZE) {
				std::shared_ptr<TreeModel> tree = std::make_shared<TreeModel>(chunk_texture, glm::vec3{ x, WORLD_MAX_HEIGHT, z });
				add_chunk(tree);
			}
		}
	}*/
}

void World::add_chunk(const std::shared_ptr<ChunkModel>& chunk) {
	m_generator_mutex.lock();

	m_chunks->push_back(chunk);	
	m_all_blocks_initialized = false;

	m_generator_mutex.unlock();

}