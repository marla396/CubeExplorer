#pragma once

#include "model.h"
#include "texture/texture_atlas.h"

class BlockModel : public Model {
public:
	BlockModel(const std::shared_ptr<TextureAtlas<32, 32>>& texture);
private:
	void generate_geometry();
};