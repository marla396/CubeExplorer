#include <utility>

#include "world/world.h"
#include "models/tree_model.h"
#include "config_file.h"
#include "resources.h"

World::World(uint32_t seed) : m_all_blocks_initialized(false), m_seed(seed) {

	m_sun = std::make_shared<Light>(glm::vec3{ WORLD_SIZE * CHUNK_SIZE * 1.2f, WORLD_MAX_HEIGHT * 1.5f, WORLD_SIZE * CHUNK_SIZE * 1.2f }, glm::vec3{ 1.0f, 1.0f, 1.0f });

	m_generator = std::make_shared<FFTNoiseGenerator>(seed, 0.00001f, 2.0f, 1.0f);

	m_chunks = std::make_shared<std::vector<std::shared_ptr<ChunkModel>>>();
	m_entities = std::make_shared<std::vector<std::shared_ptr<ChunkModel>>>();
	m_trees = std::make_shared<std::vector<std::shared_ptr<ChunkModel>>>();
	m_water_models = std::make_shared<std::vector<std::shared_ptr<WaterModel>>>();
}

World::~World() {

}

void World::update(float time, float dt, Camera& camera) {

	if (!m_all_blocks_initialized) {

		bool initialized = true;

		for (auto& chunk : *m_chunks) {
			if (!chunk->is_initialized()) {
				chunk->gpu_init();
				initialized = false;
			}
		}

		for (auto& tree : *m_trees) {
			if (!tree->is_initialized()) {
				tree->gpu_init();
				initialized = false;
			}
		}

		m_all_blocks_initialized = initialized;
	}
	else {
		auto pos = camera.get_position();
		std::sort(m_trees->begin(), m_trees->end(), [&pos](const auto& lhs, const auto& rhs) {
			return glm::distance(pos, lhs->get_position()) > glm::distance(pos, rhs->get_position());
		});
	}

	for (auto& s : m_sheep) {
		s->update(shared_from_this(), camera, dt);
	}

	//time = -(50 * PI) / 4;
	//time = 0.0f;

	float r = WORLD_CENTER.x * 50.2f;
	float phase = PI / 5.0f;

	glm::vec3 new_pos = { WORLD_CENTER.x + r * std::cos(time / 500.0f + phase), WORLD_CENTER.y + r * std::sin(time / 500.0f + phase), WORLD_CENTER.z + r * std::cos(time / 500.0f + phase) };

	//glm::vec3 new_pos = { 0.0f, 0.0f, 0.0f };

	m_sun->set_position(new_pos);
	m_sun->update(camera, time);
}

void World::regenerate_world(const std::shared_ptr<ITexture>& chunk_texture){
	lock();
	clear_world();
	set_seed(std::random_device{}());
	generate_world(chunk_texture);
	unlock();
}

void World::clear_world() {
	m_chunks->clear();
	m_entities->clear();
	m_water_models->clear();
	m_trees->clear();
}

bool World::is_initialized() const {
	return m_all_blocks_initialized;
}

float World::height_at(float x, float z) const {
	if (x >= 0.0f && z >= 0.0f && x + 0.5f < static_cast<float>(WORLD_SIZE * CHUNK_SIZE) && z + 0.5f < static_cast<float>(WORLD_SIZE * CHUNK_SIZE)) {
		return std::floor(m_height_map->noise[static_cast<int>(x + 0.5f)][static_cast<int>(z + 0.5f)]);
	}
	return 0.0f;
}

void World::generate_world(const std::shared_ptr<ITexture>& chunk_texture) {

	read_configuration();

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

	/*for (int i = 0; i < WORLD_SHEEP_AMOUNT; i++) {
	auto sheep = std::make_shared<Sheep>(chunk_texture);
	sheep->set_position(get_sheep_spawn_point());
	m_sheep.push_back(sheep);
	m_entities->push_back(std::dynamic_pointer_cast<SheepModel>(sheep->get_model()));
	}*/

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

std::shared_ptr<std::vector<std::shared_ptr<ChunkModel>>> World::get_trees() const {
	return m_trees;
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

	std::default_random_engine rng;
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	std::vector<glm::vec3> tree_positions;

	auto make_tree = [&rng, &dist, &tree_positions](const glm::vec3& pos) {

		for (const auto& t : tree_positions) {
			if (glm::distance(pos, t) < 10.0f)
				return false;
		}

		return dist(rng) > 0.99;
	};

	auto make_plant = [&rng, &dist]() {
		return dist(rng) > 0.99;
	};

	auto get_plant_type = [&rng, &dist]() {
		auto r = dist(rng);

		if (r > 0.5)
			return TreeModel::PLANT_1;
		else
			return TreeModel::PLANT_2;
	};

	glm::vec3 previous_tree = glm::vec3{ std::numeric_limits<float>::max() };

	for (int x = x_start; x < x_end; x++) {
		for (int z = 0; z < WORLD_SIZE; z++) {
			for (int y = 0; y < WORLD_MAX_HEIGHT / CHUNK_SIZE; y++) {

				glm::vec3 chunk_pos = { x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE };

				BlockMap block_map = {};

				block_map.set_global_height(y * CHUNK_SIZE);

				for (int cx = 0; cx < CHUNK_SIZE; cx++) {
					for (int cz = 0; cz < CHUNK_SIZE; cz++) {
						int noise = static_cast<int>(m_height_map->noise[x * CHUNK_SIZE + cx][z * CHUNK_SIZE + cz]);
						int height = noise - y * CHUNK_SIZE;

						for (int cy = 0; cy < height && cy < CHUNK_SIZE; cy++) {
							bool is_top = noise == y * CHUNK_SIZE + cy + 1;

							block_map.insert_block(cx, cy, cz, height < CHUNK_SIZE + 1);

							if (is_top && y * CHUNK_SIZE + cy > WORLD_BEACH_HEIGHT) {
								glm::vec3 pos = chunk_pos + glm::vec3{ cx, cy + 1, cz };
								if (make_tree(pos)) {
									previous_tree = pos;
									auto tree = std::make_shared<TreeModel>(TreeModel::TREE, chunk_texture, pos);
									add_tree(tree);
									tree_positions.push_back(pos);
								}
								else if (make_plant()) {
									auto plant = std::make_shared<TreeModel>(get_plant_type(), chunk_texture, pos);
									add_tree(plant);
								}
							}

						}
					}
				}

				auto chunk = std::make_shared<ChunkModel>(chunk_texture, chunk_pos, block_map);
				add_chunk(chunk);


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

void World::add_tree(const std::shared_ptr<ChunkModel>& tree) {
	m_generator_mutex.lock();

	m_trees->push_back(tree);
	m_all_blocks_initialized = false;

	m_generator_mutex.unlock();
}

glm::vec3 World::get_sheep_spawn_point() const {
	std::default_random_engine rng(m_seed);
	std::uniform_int_distribution<int> dist(0, CHUNK_SIZE * WORLD_SIZE - 1);

	auto has_sheep = [this](const glm::vec3& p) {
		for (const auto& s : m_sheep) {
			if (p == s->get_position()) {
				return true;
			}
		}
		return false;
	};

	for (;;) {
		int x = dist(rng);
		int z = dist(rng);
		float y = m_height_map->noise[x][z];

		if (y > WORLD_WATER_HEIGHT) {
			glm::vec3 pos = { static_cast<float>(x), std::floor(y), static_cast<float>(z) };

			if (!has_sheep(pos)) {
				return pos;
			}
		}
	}
}

void World::read_configuration() const {
	ConfigFile cfg(Resources::resolve_root_path("world_config.cfg"));

	WORLD_MAX_HEIGHT = cfg.get<int>("WORLD_MAX_HEIGHT");
	WORLD_WATER_HEIGHT = cfg.get<float>("WORLD_WATER_HEIGHT");
	WORLD_BEACH_HEIGHT = cfg.get<float>("WORLD_BEACH_HEIGHT");
	WATER_QUAD_DIMENSION = cfg.get<int>("WATER_QUAD_DIMENSION");

	float fft_cutoff = cfg.get<float>("FFT_CUTOFF_FREQUENCY");
	float fft_order = cfg.get<float>("FFT_FILTER_ORDER");

	m_generator->set_cutoff(fft_cutoff);
	m_generator->set_order(fft_order);
}