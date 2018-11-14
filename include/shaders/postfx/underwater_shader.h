#pragma once

#include "../shader.h"
#include "../multi_tex_shader.h"

class UnderwaterShader : public Shader, public MultiTexShader<3> {
public:
	UnderwaterShader();

	UnderwaterShader(const UnderwaterShader&) = delete;
	UnderwaterShader(const UnderwaterShader&&) = delete;

	void upload_time(float time) const;
	void upload_screen_dimensions(const glm::vec2& dim) const;

private:
	void get_uniform_locations() override;

	int m_time_location;
	int m_screen_dimensions_location;
};