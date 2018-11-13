#pragma once

#include "shader.h"
#include "mvp_shader.h"
#include "multi_tex_shader.h"
#include "light_shader.h"
#include "clip_shader.h"

class EntityShader : public Shader, public MVPShader, public MultiTexShader<2>, public LightShader, public ClipShader {
public:
	EntityShader();
	EntityShader(bool depth);
	
	EntityShader(const EntityShader&) = delete;
	EntityShader(const EntityShader&&) = delete;

	void upload_shadow_transform(const glm::mat4& transform) const;
private:
	void get_uniform_locations() override;

	int m_shadow_transform_location;
	bool m_depth;
};