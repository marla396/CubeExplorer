#pragma once

#include "../shader.h"
#include "../tex_shader.h"

class PlainShader : public Shader, public TexShader {
public:
	PlainShader();

	PlainShader(const PlainShader&) = delete;
	PlainShader(const PlainShader&&) = delete;
private:
	void get_uniform_locations() override;
};