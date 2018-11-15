#pragma once

#include "extension_shader.h"

class ShadowShader : public ExtensionShader {
public:
	ShadowShader(Shader* parent);
	virtual ~ShadowShader();

	void upload_shadow_transform_low(const glm::mat4& transform) const;
	void upload_shadow_transform_high(const glm::mat4& transform) const;
	void upload_camera_position(const glm::vec3& position) const;

protected:
	void get_uniform_locations();
private:
	int m_shadow_transform_low_location;
	int m_shadow_transform_high_location;
	int m_camera_position_location;
};