#pragma once

#include "shader.h"
#include "multi_tex_shader.h"

class InvertShader : public Shader, public MultiTexShader<2> {
public:
	InvertShader();

	InvertShader(const InvertShader&) = delete;
	InvertShader(const InvertShader&&) = delete;
private:
	void get_uniform_locations() override;
};