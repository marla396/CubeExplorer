#pragma once

#include "renderer.h"
#include "models/chunk_model.h"
#include "shaders/chunk_shader.h"

class ChunkRenderer : public Renderer<ChunkShader, ChunkModel> {
public:
	ChunkRenderer();

	void render(const std::vector<std::shared_ptr<ChunkModel>>& models, Camera& camera) override;
private:
};