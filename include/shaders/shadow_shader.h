#pragma once

#include <array>

#include "extension_shader.h"
#include "world/light.h"
#include "world/world_const.h"


class ShadowShader : public ExtensionShader {
public:
	ShadowShader(Shader* parent);
	virtual ~ShadowShader();

	void upload_shadow_maps(int first) const;
	void upload_shadow_transforms(const std::shared_ptr<Light>& light) const;
	void upload_shadow_cascade_end(const Camera& camera, const std::shared_ptr<Light>& light) const;
	void upload_view_matrix(const glm::mat4& matrix) const;
	void upload_projection_depth(const glm::vec2& projection_depth) const;

protected:
	void get_uniform_locations();
private:
	std::array<int, SHADOW_CASCADES> m_shadow_transform_locations;
	std::array<int, SHADOW_CASCADES> m_shadow_map_locations;
	std::array<int, SHADOW_CASCADES> m_shadow_cascade_end_locations;
	int m_view_matrix_location;
	int m_projection_depth_location;
};