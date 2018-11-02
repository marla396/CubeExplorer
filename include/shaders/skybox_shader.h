#pragma once

#include "mvp_shader.h"
#include "tex_shader.h"
#include "clip_shader.h"

class SkyboxShader : public Shader, public MVPShader, public TexShader {
public:
	SkyboxShader();

	SkyboxShader(const SkyboxShader&) = delete;
	SkyboxShader(const SkyboxShader&&) = delete;

private:
	void get_uniform_locations() override;
};