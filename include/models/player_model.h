#pragma once

#include "chunk_model.h"

class PlayerModel : public ChunkModel{
public:
	PlayerModel(const std::shared_ptr<ITexture>& texture);

	void generate_geometry();

	void generate_block(int x, int y, int z, const glm::vec3& scale, const glm::vec3& offset, const std::array<ChunkModel::BlockTexture, 6>& tex);
private:
};