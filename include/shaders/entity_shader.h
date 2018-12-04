#pragma once

#include "shader.h"
#include "mvp_shader.h"
#include "multi_tex_shader.h"
#include "light_shader.h"
#include "clip_shader.h"
#include "shadow_shader.h"

class EntityShader : public Shader, public MVPShader, public MultiTexShader<1>, public LightShader, public ClipShader, public ShadowShader {
public:
	EntityShader();
	EntityShader(bool depth);
	
	EntityShader(const EntityShader&) = delete;
	EntityShader(const EntityShader&&) = delete;

private:
	void get_uniform_locations() override;

	bool m_depth;
};