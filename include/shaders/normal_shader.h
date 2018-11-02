#pragma once

#include "shader.h"
#include "multi_tex_shader.h"

class NormalShader : public Shader, public MultiTexShader<2> {
public:
	NormalShader();
	
	NormalShader(const NormalShader&) = delete;
	NormalShader(const NormalShader&&) = delete;

	void upload_N(int n) const;
	void upload_strength(float strength) const;

private:
	void get_uniform_locations() override;

	int m_N_location;
	int m_strength_location;
};