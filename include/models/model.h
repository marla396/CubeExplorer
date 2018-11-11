#pragma once

#include <memory>
#include <vector>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "gl.h"
#include "texture/itexture.h"

class Model {
public:
    Model(bool lazy_init = false);
    virtual ~Model();

    void bind() const;
    void unbind() const;
    void upload_geometry();
	void clear_geometry();
	
	void set_texture(const std::shared_ptr<ITexture>& texture);
	void bind_texture(GLenum texture_unit) const;

	glm::mat4 get_model_matrix() const;

	glm::vec3 get_position() const;
	void set_position(const glm::vec3& position);

	glm::vec3 get_scale() const;
	void set_scale(const glm::vec3& scale);

	size_t get_indices_count() const;

	virtual float get_bounding_radius() const;
	virtual glm::vec3 get_center() const;

protected:
	void late_init();

	std::vector<float> m_vertices;
	std::vector<uint32_t> m_indices;
	std::vector<float> m_tex_coords;
	std::vector<float> m_normals;

	std::shared_ptr<ITexture> m_texture;
	glm::vec3 m_position;
	glm::vec3 m_scale;
	
	int m_vertices_dimensions;
private:
	virtual uint32_t add_vbo(int dimension, const std::vector<float>& data);
	uint32_t add_ibo(const std::vector<uint32_t>& data);


	size_t m_indices_count;
	bool m_has_tex_coords;
	bool m_has_normals;

    uint32_t m_vao;
    uint32_t m_vbo;
	uint32_t m_tbo;
	uint32_t m_nbo;
    uint32_t m_ibo;
	uint32_t m_vbo_index;
	uint32_t m_ibo_index;
};