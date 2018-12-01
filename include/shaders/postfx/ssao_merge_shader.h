#pragma once

#include "../shader.h"
#include "../multi_tex_shader.h"

class SSAOMergeShader : public Shader, public MultiTexShader<3> {
public:
	SSAOMergeShader();

	SSAOMergeShader(const SSAOMergeShader&) = delete;
	SSAOMergeShader(const SSAOMergeShader&&) = delete;

private:
	void get_uniform_locations() override;
};
