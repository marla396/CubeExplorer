#include "renderers/block_renderer.h"

BlockRenderer::BlockRenderer() {
	
}

void BlockRenderer::render(const std::vector<std::shared_ptr<BlockModel>>& models, Camera& camera, const std::shared_ptr<Light>&) {

	if (models.empty()) {
		return;
	}

	m_shader->bind();

	m_shader->upload_tex_unit(0);
	m_shader->upload_view_matrix(camera.get_view_matrix());
	m_shader->upload_projection_matrix(camera.get_projection_matrix());

	GLC(glEnable(GL_DEPTH_TEST));
	GLC(glEnable(GL_CULL_FACE));
	GLC(glFrontFace(GL_CW));

	for (const auto& model : models) {
		model->bind();
		model->bind_texture(GL_TEXTURE0);
		m_shader->upload_model_matrix(model->get_model_matrix());
		DRAW_CALL(GLC(glDrawElements(GL_TRIANGLES, model->get_indices_count(), GL_UNSIGNED_INT, nullptr)));
	}

	GLC(glDisable(GL_CULL_FACE));
	GLC(glDisable(GL_DEPTH_TEST));

}