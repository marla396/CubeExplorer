#pragma once

#include "shader.h"
#include "multi_tex_shader.h"

class DuDvShader : public Shader, public MultiTexShader<2> {
public:
	DuDvShader();

	DuDvShader(const DuDvShader&) = delete;
	DuDvShader(const DuDvShader&&) = delete;

private:
	void get_uniform_locations() override;
};