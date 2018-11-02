#include "models/model.h"

Model::Model() :
	m_vertices_dimensions(3), m_indices_count(0) , m_has_normals(false), m_has_tex_coords(false), m_dirty(true), m_vbo_index(0), m_ibo_index(0) {

	m_position = { 0.0f, 0.0f, 0.0f };
	m_scale = { 1.0f, 1.0f, 1.0f };
    GLC(glGenVertexArrays(1, &m_vao));
}

Model::~Model(){
	//TODO: remove buffers
	GLC(glDeleteVertexArrays(1, &m_vao));
}

void Model::bind() const{
    GLC(glBindVertexArray(m_vao));
}

void Model::unbind() const{
    GLC(glBindVertexArray(0));
}

void Model::set_texture(const std::shared_ptr<ITexture>& texture){
	m_texture = texture;
}

void Model::bind_texture(GLenum texture_unit) const{
	m_texture->bind(texture_unit);
}

void Model::upload_geometry(){
	if (m_vertices.empty())
		return;

	bind();
	m_vbo = add_vbo(m_vertices_dimensions, m_vertices);
	m_ibo = add_ibo(m_indices);

	if (!m_tex_coords.empty()) {
		m_has_tex_coords = true;
		m_tbo = add_vbo(2, m_tex_coords);
	}

	m_indices_count = m_indices.size();
	m_dirty = false;
}

void Model::clear_geometry() {
	m_vertices.clear();
	m_indices.clear();
	m_tex_coords.clear();
}

glm::mat4 Model::get_model_matrix() const {

	auto matrix = glm::identity<glm::mat4>();

	matrix = glm::translate(matrix, m_position);
	matrix = glm::scale(matrix, m_scale);

	return matrix;
}

glm::vec3 Model::get_position() const {
	return m_position;
}

void Model::set_position(const glm::vec3& position) {
	m_position = position;
}

glm::vec3 Model::get_scale() const {
	return m_scale;
}

void Model::set_scale(const glm::vec3& scale) {
	m_scale = scale;
}

size_t Model::get_indices_count() const {
	return m_indices_count;
}

void Model::set_dirty(bool dirty) {
	m_dirty = dirty;
}

bool Model::has_normals() const {
	return m_has_normals;
}

bool Model::has_tex_coords() const {
	return m_has_tex_coords;
}

uint32_t Model::add_vbo(int dimension, const std::vector<float>& data){

	uint32_t vbo;

	GLC(glGenBuffers(1, &vbo));
	GLC(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLC(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW));

	GLC(glVertexAttribPointer(m_vbo_index, dimension, GL_FLOAT, GL_FALSE, 0, nullptr));
	GLC(glEnableVertexAttribArray(m_vbo_index++));

	return vbo;
} 


uint32_t Model::add_ibo(const std::vector<uint32_t>& data) {

	uint32_t ibo;

	GLC(glGenBuffers(1, &ibo));
	GLC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLC(glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(uint32_t), data.data(), GL_STATIC_DRAW));
	m_ibo_index++;

	return ibo;
}
