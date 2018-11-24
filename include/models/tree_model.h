#pragma once

#include <random>

#include "chunk_model.h"

class TreeModel : public ChunkModel {
public:
	TreeModel(const std::shared_ptr<ITexture>& texture, const glm::vec3& position);

	void generate_geometry();
private:
	void generate_trunk_blocks(int x, int y, int z, int depth);
	void generate_branch_blocks(int x, int y, int z, int depth);

	void generate_trunk(int x, int y, int z);
	void generate_branch(int x, int y, int z);

	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist;
};