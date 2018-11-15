#pragma once

#include "../shader.h"
#include "../multi_tex_shader.h"

class FXAAShader : public Shader, public MultiTexShader<2>{
public:
	FXAAShader();

	FXAAShader(const FXAAShader&) = delete;
	FXAAShader(const FXAAShader&&) = delete;

	void upload_threshold(float threshold) const;

private:
	void get_uniform_locations() override;

	int m_threshold_location;
};
