#pragma once

#include <random>

#include "chunk_model.h"

class TreeModel : public ChunkModel {
public:
	enum TreeType {
		TREE,
		PLANT_1,
		PLANT_2,
	};

	TreeModel(TreeType type, const std::shared_ptr<ITexture>& texture, const glm::vec3& position);

	void generate_geometry(TreeType type);
private:
	void generate_trunk_blocks(int x, int y, int z, int depth);

	void generate_trunk(int x, int y, int z);
	void generate_branch(int x, int y, int z);
	void generate_plant(int x, int y, int z, TreeType type);

	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist;
};