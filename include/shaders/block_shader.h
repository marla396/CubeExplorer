#pragma once

#include "mvp_shader.h"
#include "tex_shader.h"

class BlockShader : public Shader, public MVPShader, public TexShader {
public:
	BlockShader();
	~BlockShader();

	BlockShader(const BlockShader&) = delete;
	BlockShader(const BlockShader&&) = delete;

private:
	void get_uniform_locations() override;
};