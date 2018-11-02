#include "hud/hud_texture.h"

#include <glm/ext/matrix_transform.hpp>

const float quad_2d[] = {
	-1.0f, -1.0f,
	-1.0f, 1.0f,
	1.0f, 1.0f,
	-1.0f, -1.0f,
	1.0f, -1.0f,
	1.0f, 1.0f,
};

const float quad_2d_tex[] = {
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
};

HUDTexture::HUDTexture(const std::shared_ptr<ITexture>& tex) {

	m_texture = tex;
	m_transformation_matrix = glm::identity<glm::mat4>();

	GLC(glGenVertexArrays(1, &m_vao));

	bind();

	GLC(glGenBuffers(1, &m_vbo));
	GLC(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GLC(glBufferData(GL_ARRAY_BUFFER, sizeof(quad_2d), quad_2d, GL_STATIC_DRAW));

	GLC(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr));
	GLC(glEnableVertexAttribArray(0));

	GLC(glGenBuffers(1, &m_tbo));
	GLC(glBindBuffer(GL_ARRAY_BUFFER, m_tbo));
	GLC(glBufferData(GL_ARRAY_BUFFER, sizeof(quad_2d_tex), quad_2d_tex, GL_STATIC_DRAW));

	GLC(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr));
	GLC(glEnableVertexAttribArray(1));
}

HUDTexture::~HUDTexture() {
	GLC(glDeleteBuffers(1, &m_vbo))
	GLC(glDeleteBuffers(1, &m_tbo))
	GLC(glDeleteVertexArrays(1, &m_vao))
}

void HUDTexture::bind() const {
	GLC(glBindVertexArray(m_vao));
}


void HUDTexture::render() {
	bind();
	m_texture->bind(GL_TEXTURE0);
	GLC(glEnable(GL_BLEND));
	GLC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	DRAW_CALL(GLC(glDrawArrays(GL_TRIANGLES, 0, sizeof(quad_2d) / sizeof(float))));

	GLC(glDisable(GL_BLEND));
}

void HUDTexture::set_position(const glm::vec2& p) {
	m_position = { p.x, p.y, 0.0f };
	update_transformation_matrix();
}

void HUDTexture::set_size(const glm::vec2& size) {
	m_size = { size.x, size.y, 0.0f };
	update_transformation_matrix();
}

glm::mat4 HUDTexture::get_transformation_matrix() const {
	return m_transformation_matrix;
}

void HUDTexture::update_transformation_matrix() {
	m_transformation_matrix = glm::identity<glm::mat4>();
	m_transformation_matrix = glm::translate(m_transformation_matrix, m_position);
	m_transformation_matrix = glm::scale(m_transformation_matrix, m_size);
}

