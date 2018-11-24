#include <array>

#include "models/tree_model.h"

TreeModel::TreeModel(const std::shared_ptr<ITexture>& texture, const glm::vec3& position) 
	: ChunkModel(texture, position, true), m_rng(std::random_device{}()), m_dist(0.0, 1.0){
	generate_geometry();
}

void TreeModel::generate_geometry() {
	generate_trunk(CHUNK_SIZE / 2, 0, CHUNK_SIZE / 2);
	generate_trunk(CHUNK_SIZE / 2, 1, CHUNK_SIZE / 2);
	generate_trunk(CHUNK_SIZE / 2, 2, CHUNK_SIZE / 2);
	generate_trunk(CHUNK_SIZE / 2, 3, CHUNK_SIZE / 2);

	m_position -= glm::vec3 {CHUNK_SIZE / 2.0f, 0.0f, CHUNK_SIZE / 2.0f};
	
	generate_trunk_blocks(CHUNK_SIZE / 2, 4, CHUNK_SIZE / 2, 1);
}

void TreeModel::generate_trunk_blocks(int x, int y, int z, int depth) {

	const float HEIGHT_COEFF = 1.0f;
	const float PLANE_COEFF = 1.0f;

	if (depth > 6)
		return;

	auto r = m_dist(m_rng);

	if (depth > 2)
		generate_branch(x, y, z);
	else
		generate_trunk(x, y, z);

	if (r < (1.0f / depth) * HEIGHT_COEFF) {
		generate_trunk_blocks(x, y + 1, z, depth + 1);
	} else {
		generate_branch(x, y + 1, z);
	}

	r = m_dist(m_rng);

	if (r < (1.0f / depth) * PLANE_COEFF) {
		generate_trunk_blocks(x + 1, y, z, depth + 1);
	} else {
		generate_branch(x + 1, y, z);
	}

	r = m_dist(m_rng);

	if (r < (1.0f / depth) * PLANE_COEFF) {
		generate_trunk_blocks(x - 1, y, z, depth + 1);
	} else {
		generate_branch(x - 1, y, z);
	}

	r = m_dist(m_rng);

	if (r < (1.0f / depth) * PLANE_COEFF) {
		generate_trunk_blocks(x, y, z + 1, depth + 1);
	} else {
		generate_branch(x, y, z + 1);
	}

	r = m_dist(m_rng);

	if (r < (1.0f / depth) * PLANE_COEFF) {
		generate_trunk_blocks(x, y, z - 1, depth + 1);
	} else {
		generate_branch(x, y, z - 1);
	}
}

void TreeModel::generate_branch_blocks(int x, int y, int z, int depth) {

	
}

void TreeModel::generate_trunk(int x, int y, int z) {
	
	generate_face(x, y, z, POSITIVE, right_face);
	generate_face_texture(ChunkModel::TREE_TRUNK);
	generate_normals(right_normal);

	generate_face(x, y, z, NEGATIVE, left_face);
	generate_face_texture(ChunkModel::TREE_TRUNK);
	generate_normals(left_normal);

	generate_face(x, y, z, POSITIVE, top_face);
	generate_face_texture(ChunkModel::TREE_TRUNK_INSIDE);
	generate_normals(top_normal);

	generate_face(x, y, z, POSITIVE, front_face);
	generate_face_texture(ChunkModel::TREE_TRUNK);
	generate_normals(front_normal);

	generate_face(x, y, z, NEGATIVE, back_face);
	generate_face_texture(ChunkModel::TREE_TRUNK);
	generate_normals(back_normal);

	generate_face(x, y, z, NEGATIVE, bottom_face);
	generate_face_texture(ChunkModel::TREE_TRUNK_INSIDE);
	generate_normals(bottom_normal);
}

void TreeModel::generate_branch(int x, int y, int z) {
	generate_face(x, y, z, POSITIVE, right_face);
	generate_face_texture(ChunkModel::TREE_BRANCH);
	generate_normals(right_normal);

	generate_face(x, y, z, NEGATIVE, left_face);
	generate_face_texture(ChunkModel::TREE_BRANCH);
	generate_normals(left_normal);

	generate_face(x, y, z, POSITIVE, top_face);
	generate_face_texture(ChunkModel::TREE_BRANCH);
	generate_normals(top_normal);

	generate_face(x, y, z, POSITIVE, front_face);
	generate_face_texture(ChunkModel::TREE_BRANCH);
	generate_normals(front_normal);

	generate_face(x, y, z, NEGATIVE, back_face);
	generate_face_texture(ChunkModel::TREE_BRANCH);
	generate_normals(back_normal);

	generate_face(x, y, z, NEGATIVE, bottom_face);
	generate_face_texture(ChunkModel::TREE_BRANCH);
	generate_normals(bottom_normal);
}