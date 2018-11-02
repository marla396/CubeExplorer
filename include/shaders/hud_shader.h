#pragma once

#include "shader.h"
#include "tex_shader.h"

class HUDShader : public Shader, public TexShader {
public:
	HUDShader();
	~HUDShader();

	HUDShader(const HUDShader&) = delete;
	HUDShader(const HUDShader&&) = delete;

	void upload_transformation_matrix(const glm::mat4& m) const;
private:
	void get_uniform_locations() override;

	int32_t m_transformation_matrix_location;
};