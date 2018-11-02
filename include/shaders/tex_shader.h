#pragma once

#include "extension_shader.h"

class TexShader : public ExtensionShader{
public:
	TexShader(Shader* parent);
	virtual ~TexShader();
	void upload_tex_unit(int i) const;

protected:
	void get_uniform_locations() override;
private:
	int32_t m_tex_unit_location;
};