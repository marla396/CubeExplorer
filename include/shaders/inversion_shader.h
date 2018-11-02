#pragma once

#include "shader.h"
#include "multi_tex_shader.h"

class InversionShader : public Shader, public MultiTexShader<3> {
public:
	InversionShader();

	InversionShader(const InversionShader&) = delete;
	InversionShader(const InversionShader&&) = delete;

	void upload_N(int n) const;
	void upload_pingpong(int pingpong) const;

private:
	void get_uniform_locations() override;

	int m_N_location;
	int m_pingpong_location;
};