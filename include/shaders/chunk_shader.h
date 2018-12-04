#pragma once

#include "mvp_shader.h"
#include "multi_tex_shader.h"
#include "clip_shader.h"
#include "light_shader.h"
#include "shadow_shader.h"

class ChunkShader : public Shader, public MVPShader, public MultiTexShader<1>, public ClipShader, public LightShader, public ShadowShader {
public:
	ChunkShader();
	ChunkShader(bool depth);
	~ChunkShader();

	ChunkShader(const ChunkShader&) = delete;
	ChunkShader(const ChunkShader&&) = delete;

private:
	void get_uniform_locations() override;

	bool m_depth;
};