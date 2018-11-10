#pragma once

#include "mvp_shader.h"
#include "multi_tex_shader.h"
#include "clip_shader.h"
#include "light_shader.h"

class ChunkShader : public Shader, public MVPShader, public MultiTexShader<2>, public ClipShader, public LightShader {
public:
	ChunkShader();
	ChunkShader(bool depth);
	~ChunkShader();

	ChunkShader(const ChunkShader&) = delete;
	ChunkShader(const ChunkShader&&) = delete;

	void upload_shadow_transform(const glm::mat4& transform) const;

private:
	void get_uniform_locations() override;

	int m_shadow_transform_location;
	bool m_depth;
};