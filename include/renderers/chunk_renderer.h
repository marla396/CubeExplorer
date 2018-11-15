#pragma once

#include "renderer.h"
#include "models/chunk_model.h"
#include "shaders/chunk_shader.h"

class ChunkRenderer : public Renderer<ChunkShader, ChunkModel> {
public:
	ChunkRenderer();

	void render(const std::vector<std::shared_ptr<ChunkModel>>& models, Camera& camera, const std::shared_ptr<Light>& light) override;
	void render_depth(const std::vector<std::shared_ptr<ChunkModel>>& models, Camera& camera, const std::shared_ptr<Light>& light, Light::ShadowMapQuality quality);
private:
	std::unique_ptr<ChunkShader> m_depth_shader;
};