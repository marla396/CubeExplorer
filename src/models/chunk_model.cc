#include "models/chunk_model.h"
#include "world/world.h"

#include <array>

ChunkModel::ChunkModel(const std::shared_ptr<ITexture>& texture, const glm::vec3& position) {
	set_position(position);
	set_texture(texture);
}

ChunkModel::ChunkModel(const std::shared_ptr<ITexture>& texture, const glm::vec3& position, const BlockMap& block_map)
	: Model(true), m_block_map(block_map), m_initialized(false) {
	
	set_position(position);
	set_texture(texture);
	generate_geometry(block_map);
}

ChunkModel::~ChunkModel() {

}

void ChunkModel::gpu_init() {
	
	late_init();
	upload_geometry();
	clear_geometry();
	m_initialized = true;
}

bool ChunkModel::is_initialized() const {
	return m_initialized;
}

void ChunkModel::generate_geometry(const BlockMap& block_map) {
	
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int y = 0; y < CHUNK_SIZE; y++) {
				if (!occluded(block_map, x, y, z)) {

					bool is_top = y == CHUNK_SIZE - 1 || !block_map(x, y + 1, z);

					generate_block(block_map, x, y, z, is_top);
				}
			}
		}
	}
}

BlockMap ChunkModel::get_block_map() const {
	return m_block_map;
}

glm::vec3 ChunkModel::get_center() const {
	return m_position + glm::vec3{ static_cast<float>(CHUNK_SIZE / 2), static_cast<float>(CHUNK_SIZE / 2), static_cast<float>(CHUNK_SIZE / 2) };
}

float ChunkModel::get_bounding_radius() const {
	return std::sqrt(2.0f) * CHUNK_SIZE;
}

bool ChunkModel::occluded(const BlockMap& block_map, int x, int y, int z) const{

	if (!block_map(x, y, z))
		return true;
	
	if (x == 0 || y == 0 || z == 0)
		return false;

	if (x == CHUNK_SIZE - 1 || y == CHUNK_SIZE - 1 || z == CHUNK_SIZE - 1)
		return false;

	int n = 0;

	if (block_map(x + 1, y, z)) {
		n++;
	}

	if (block_map(x - 1, y, z)) {
		n++;
	}

	if (block_map(x, y + 1, z)) {
		n++;
	}

	if (block_map(x, y - 1, z)) {
		n++;
	}
	if (block_map(x, y, z + 1)) {
		n++;
	}

	if (block_map(x, y, z - 1)) {
		n++;
	}

	return n == 6;

}

void ChunkModel::generate_block(const BlockMap& block_map, int x, int y, int z, bool is_top_block) {

	auto block = block_map(x, y, z);

	if (!(x < CHUNK_SIZE - 1 && block_map(x + 1, y, z))) {
		generate_face(x, y, z, POSITIVE, right_face);
		if (block_map.get_global_height() + y < WORLD_BEACH_HEIGHT)
			generate_face_texture(SAND);
		else if (!is_top_block || !block->is_top_chunk())
			generate_face_texture(SOIL);
		else
			generate_face_texture(GRASS_SOIL_BORDER);
		generate_normals(right_normal);
	}

	if (!(x > 0 && block_map(x - 1, y, z))) {
		generate_face(x, y, z, NEGATIVE, left_face);
		if (block_map.get_global_height() + y < WORLD_BEACH_HEIGHT)
			generate_face_texture(SAND);
		else if (!is_top_block || !block->is_top_chunk())
			generate_face_texture(SOIL);
		else
			generate_face_texture(GRASS_SOIL_BORDER);
		generate_normals(left_normal);
	}

	if (!(y < CHUNK_SIZE - 1 && block_map(x, y + 1, z))) {
		generate_face(x, y, z, POSITIVE, top_face);
		if (block_map.get_global_height() + y < WORLD_BEACH_HEIGHT)
			generate_face_texture(SAND);
		else
			generate_face_texture(GRASS);
		generate_normals(top_normal);

	}

	if (!(z < CHUNK_SIZE - 1 && block_map(x, y, z + 1))) {
		generate_face(x, y, z, POSITIVE, front_face);
		if (block_map.get_global_height() + y < WORLD_BEACH_HEIGHT)
			generate_face_texture(SAND);
		else if (!is_top_block || !block->is_top_chunk())
			generate_face_texture(SOIL);
		else
			generate_face_texture(GRASS_SOIL_BORDER);
		generate_normals(front_normal);


	}

	if (!(z > 0 && block_map(x, y, z - 1))) {
		generate_face(x, y, z, NEGATIVE, back_face);
		if (block_map.get_global_height() + y < WORLD_BEACH_HEIGHT)
			generate_face_texture(SAND);
		else if (!is_top_block || !block->is_top_chunk())
			generate_face_texture(SOIL);
		else
			generate_face_texture(GRASS_SOIL_BORDER);
		generate_normals(back_normal);
	}

	if (!(y > 0 && block_map(x, y - 1, z))) {
		generate_face(x, y, z, NEGATIVE, bottom_face);
		if (block_map.get_global_height() + y < WORLD_BEACH_HEIGHT)
			generate_face_texture(SAND);
		else
			generate_face_texture(SOIL);
		generate_normals(back_normal);
	}
}

void ChunkModel::generate_face(int x, int y, int z, FaceDirection dir, const std::array<float, 12>& face_vertices, const glm::vec3& scale, const glm::vec3& offset) {
	uint32_t start = m_vertices.size() / 3;
	for (size_t i = 0; i < face_vertices.size(); i += 3) {
		m_vertices.push_back(face_vertices[i] * scale.x + offset.x + x);
		m_vertices.push_back(face_vertices[i + 1] * scale.y + offset.y + y);
		m_vertices.push_back(face_vertices[i + 2] * scale.z + offset.z + z);
	}

	switch (dir) {
	case POSITIVE:
		m_indices.push_back(start + 0);
		m_indices.push_back(start + 1);
		m_indices.push_back(start + 2);
		m_indices.push_back(start + 1);
		m_indices.push_back(start + 3);
		m_indices.push_back(start + 2);
		break;
	case NEGATIVE:
		m_indices.push_back(start + 0);
		m_indices.push_back(start + 2);
		m_indices.push_back(start + 1);
		m_indices.push_back(start + 1);
		m_indices.push_back(start + 2);
		m_indices.push_back(start + 3);
		break;
	}
}

void ChunkModel::generate_face_texture(ChunkModel::BlockTexture texture) {

	int texture_x = 0;
	int texture_y = 0;

	switch (texture) {
	case GRASS:
		texture_x = 0;
		texture_y = 0;
		break;
	case GRASS_SOIL_BORDER:
		texture_x = 1;
		texture_y = 0;
		break;
	case SOIL:
		texture_x = 2;
		texture_y = 0;
		break;
	case SAND:
		texture_x = 2;
		texture_y = 1;
		break;
	case TREE_TRUNK_INSIDE:
		texture_x = 0;
		texture_y = 1;
		break;
	case TREE_BRANCH:
		texture_x = 1;
		texture_y = 1;
		break;
	case TREE_TRUNK:
		texture_x = 3;
		texture_y = 0;
		break;
	case PLAYER_HEAD_FRONT:
		texture_x = 0;
		texture_y = 2;
		break;
	case PLAYER_HEAD_BACK:
		texture_x = 2;
		texture_y = 2;
		break;
	case PLAYER_HEAD_SIDE:
		texture_x = 3;
		texture_y = 2;
		break;
	case PLAYER_HEAD_BOTTOM:
		texture_x = 0;
		texture_y = 3;
		break;
	case PLAYER_TORSO:
		texture_x = 3;
		texture_y = 1;
		break;
	case PLAYER_LEGS:
		texture_x = 1;
		texture_y = 2;
		break;
	case PLAYER_LEGS_BOTTOM:
		texture_x = 1;
		texture_y = 3;
		break;
	case PLAYER_ARM:
		texture_x = 2;
		texture_y = 3;
		break;
	default:
		LOG("Invalid texture enum" << std::endl);
		break;
	}

	auto coords = m_texture->get_texture_coords(texture_x, texture_y);
	m_tex_coords.insert(m_tex_coords.end(), coords.begin(), coords.end());
	
}

void ChunkModel::generate_normals(const std::array<float, 3>& normals) {
	for (int i = 0; i < 4; i++)
		m_normals.insert(m_normals.end(), normals.begin(), normals.end());
}