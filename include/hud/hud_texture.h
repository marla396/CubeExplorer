#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "gl.h"
#include "texture/itexture.h"

class HUDTexture {
public:
	HUDTexture(const std::shared_ptr<ITexture>& tex);
	~HUDTexture();

	void bind() const;

	void render();
	void set_position(const glm::vec2& p);
	void set_size(const glm::vec2& size);

	glm::mat4 get_transformation_matrix() const;
private:
	void update_transformation_matrix();

	uint32_t m_vao;
	uint32_t m_vbo;
	uint32_t m_tbo;

	std::shared_ptr<ITexture> m_texture;

	glm::vec3 m_position;
	glm::vec3 m_size;

	glm::mat4 m_transformation_matrix;
};