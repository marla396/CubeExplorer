#pragma once

#include "multi_tex_shader.h"

class H0kShader : public Shader, public MultiTexShader<6>{
public:
	H0kShader();

	H0kShader(const H0kShader&) = delete;
	H0kShader(const H0kShader&&) = delete;

	void upload_N(int n) const;
	void upload_L(int l) const;
	void upload_amplitude(float a) const;
	void upload_wind_speed(float a) const;
	void upload_wind_direction(const glm::vec2 dir) const;
	void upload_capillar_supress_factor(float c) const;

private:
	void get_uniform_locations() override;

	int m_N_location;
	int m_L_location;
	int m_amplitude_location;
	int m_wind_speed_location;
	int m_wind_direction_location;
	int m_capillar_supress_factor_location;
};