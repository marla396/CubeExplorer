#pragma once

#include "../shader.h"
#include "../multi_tex_shader.h"

class UnderwaterShader : public Shader, public MultiTexShader<2> {
public:
	UnderwaterShader();

	UnderwaterShader(const UnderwaterShader&) = delete;
	UnderwaterShader(const UnderwaterShader&&) = delete;

	void upload_time(float time) const;
	void upload_depth(float depth) const;

private:
	void get_uniform_locations() override;

	int m_time_location;
	int m_depth_location;
};