#pragma once

#include <vector>

#include "../shader.h"
#include "../tex_shader.h"

class LowpassXShader : public Shader, public TexShader {
public:
	LowpassXShader();

	LowpassXShader(const LowpassXShader&) = delete;
	LowpassXShader(const LowpassXShader&&) = delete;

	void upload_kernel(const std::vector<float>& kernel) const;
private:
	void get_uniform_locations() override;

	int m_radius_location;
	int m_offset_location;
	int m_kernel_location;
};