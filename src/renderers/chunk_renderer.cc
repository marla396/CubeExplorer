#include "renderers/chunk_renderer.h"

ChunkRenderer::ChunkRenderer() {
	
}

void ChunkRenderer::render(const std::vector<std::shared_ptr<ChunkModel>>& models, Camera& camera){
	if (models.empty()) {
		return;
	}

	m_shader->bind();

	m_shader->upload_tex_unit(0);
	m_shader->upload_view_matrix(camera.get_view_matrix());
	m_shader->upload_projection_matrix(camera.get_projection_matrix());
	m_shader->upload_clip_plane(m_clip_plane);


	GLC(glEnable(GL_DEPTH_TEST));
	GLC(glEnable(GL_CULL_FACE));
	GLC(glFrontFace(GL_CCW));
	GLC(glEnable(GL_CLIP_DISTANCE0));

	for (const auto& model : models) {
		if (model->get_indices_count() > 0) {
			model->bind();
			model->bind_texture(GL_TEXTURE0);
			m_shader->upload_model_matrix(model->get_model_matrix());
			DRAW_CALL(GLC(glDrawElements(GL_TRIANGLES, model->get_indices_count(), GL_UNSIGNED_INT, nullptr)));
		}
	}

	GLC(glDisable(GL_CULL_FACE));
	GLC(glDisable(GL_DEPTH_TEST));
	GLC(glDisable(GL_CLIP_DISTANCE0));


}