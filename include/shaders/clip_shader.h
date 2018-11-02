#pragma once

#include <glm/glm.hpp>

#include "extension_shader.h"

class ClipShader : public ExtensionShader {
public:
	ClipShader(Shader* parent);

	void upload_clip_plane(const glm::vec4& v) const;

protected:
	void get_uniform_locations() override;
private:
	int32_t m_clip_plane_location;
};