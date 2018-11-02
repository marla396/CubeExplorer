#pragma once

#include "extension_shader.h"

class MVPShader : public ExtensionShader{
public:
	MVPShader(Shader* parent);
	virtual ~MVPShader();

	void upload_model_matrix(const glm::mat4& m) const;
	void upload_view_matrix(const glm::mat4& v) const;
	void upload_projection_matrix(const glm::mat4& p) const;
protected:
	void get_uniform_locations() override;
private:
	int32_t m_model_matrix_location;
	int32_t m_view_matrix_location;
	int32_t m_projection_matrix_location;
};
