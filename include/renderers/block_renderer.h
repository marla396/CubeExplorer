#pragma once

#include "renderer.h"
#include "models/block_model.h"
#include "shaders/block_shader.h"

class BlockRenderer : Renderer<BlockShader, BlockModel> {
public:
	BlockRenderer();

	void render(const std::vector<std::shared_ptr<BlockModel>>& models, Camera& camera, const std::shared_ptr<Light>& light) override;
};