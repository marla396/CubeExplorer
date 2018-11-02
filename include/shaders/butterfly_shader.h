#pragma once

#include "shader.h"
#include "multi_tex_shader.h"

class ButterflyShader : public Shader, public MultiTexShader<3> {
public:
	ButterflyShader();

	ButterflyShader(const ButterflyShader&) = delete;
	ButterflyShader(const ButterflyShader&&) = delete;

	void upload_stage(int stage) const;
	void upload_pingpong(int pingpong) const;
	void upload_direction(int direction) const;
private:
	void get_uniform_locations() override;

	int m_stage_location;
	int m_pingpong_location;
	int m_direction_location;
};