#pragma once

#include <memory>

#include "extension_shader.h"
#include "world/light.h"

class LightShader : public ExtensionShader {
public:
	LightShader(Shader* parent);
	virtual ~LightShader();

	void upload_light_source(const std::shared_ptr<Light>& light_source);
protected:
	void get_uniform_locations() override;

private:
	int m_light_position_location;
	int m_light_color_location;
};
