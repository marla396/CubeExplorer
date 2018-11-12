#pragma once

#include "chunk_model.h"

class TreeModel : public ChunkModel {
public:
	TreeModel(const std::shared_ptr<ITexture>& texture, const glm::vec3& position);

	void generate_geometry();
private:

	void generate_trunk(int x, int y, int z);
	void generate_branch(int x, int y, int z);
};