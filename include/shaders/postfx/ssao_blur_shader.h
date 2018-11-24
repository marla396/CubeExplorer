#pragma once

#include <vector>

#include "../shader.h"
#include "../multi_tex_shader.h"

class SSAOBlurShader : public Shader, public MultiTexShader<2> {
public:
	SSAOBlurShader();

	SSAOBlurShader(const SSAOBlurShader&) = delete;
	SSAOBlurShader(const SSAOBlurShader&&) = delete;

private:
	void get_uniform_locations() override;
};