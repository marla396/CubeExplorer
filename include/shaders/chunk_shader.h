#pragma once

#include "mvp_shader.h"
#include "tex_shader.h"
#include "clip_shader.h"

class ChunkShader : public Shader, public MVPShader, public TexShader, public ClipShader {
public:
	ChunkShader();
	~ChunkShader();

	ChunkShader(const ChunkShader&) = delete;
	ChunkShader(const ChunkShader&&) = delete;

private:
	void get_uniform_locations() override;
};