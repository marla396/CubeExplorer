#pragma once

#include "renderer.h"
#include "models/skybox_model.h"
#include "shaders/skybox_shader.h"

class SkyboxRenderer : public Renderer<SkyboxShader, SkyboxModel> {
public:
	SkyboxRenderer();

	void render(const std::vector<std::shared_ptr<SkyboxModel>>& models, Camera& camera) override;
};