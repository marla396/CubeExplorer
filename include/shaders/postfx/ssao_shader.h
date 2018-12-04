#pragma once

#include <vector>

#include "../shader.h"
#include "../multi_tex_shader.h"

class SSAOShader : public Shader, public MultiTexShader<4>{
public:
	SSAOShader();

	SSAOShader(const SSAOShader&) = delete;
	SSAOShader(const SSAOShader&&) = delete;

	void upload_screen_dimensions(const glm::vec2& dimensions) const;
	void upload_kernel(const std::vector<glm::vec3>& kernel) const;
	void upload_projection_depth(const glm::vec2& depth) const;

private:
	void get_uniform_locations() override;

	int m_screen_dimensions_location;
	int m_kernel_location;
	int m_projection_depth_location;
};