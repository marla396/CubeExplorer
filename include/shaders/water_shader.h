#pragma once

#include "shader.h"
#include "mvp_shader.h"
#include "multi_tex_shader.h"
#include "light_shader.h"

class WaterShader : public Shader, public MVPShader, public MultiTexShader<8>, public LightShader {
public:
	WaterShader();
	WaterShader(bool depth);
	~WaterShader();

	WaterShader(const WaterShader&) = delete;
	WaterShader(const WaterShader&&) = delete;

	void upload_camera_position(const glm::vec3& pos) const;
	void upload_displacement_factor(float factor) const;
	void upload_water_height(float height) const;
	void upload_quad_dimension(int dim) const;
	void upload_quad_instance(const glm::vec2& instance) const;
	void upload_shadow_transform(const glm::mat4& transform) const;
private:
	void get_uniform_locations() override;

	bool m_depth;

	int m_camera_position_location;
	int m_displacement_factor_location;
	int m_water_height_location;
	int m_quad_dimension_location;
	int m_quad_instance_location;
	int m_shadow_transform_location;
};
