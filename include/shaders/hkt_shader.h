#pragma once

#include "shader.h"
#include "multi_tex_shader.h"

class HktShader : public Shader, public MultiTexShader<5> {
public:
	HktShader();

	HktShader(const HktShader&) = delete;
	HktShader(const HktShader&&) = delete;

	void upload_N(int n) const;
	void upload_L(int l) const;
	void upload_time(float time) const;

private:
	void get_uniform_locations() override;

	int m_N_location;
	int m_L_location;
	int m_time_location;
};