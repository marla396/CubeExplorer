#include "renderers/chunk_renderer.h"

ChunkRenderer::ChunkRenderer() {
	m_depth_shader = std::make_unique<ChunkShader>(true);
}

void ChunkRenderer::render(const std::vector<std::shared_ptr<ChunkModel>>& models, Camera& camera, const std::shared_ptr<Light>& light){
	if (models.empty()) {
		return;
	}

	m_shader->bind();

	m_shader->upload_tex_units({ 0 });
	m_shader->upload_view_projection_matrix(camera.get_view_projection_matrix());
	m_shader->upload_clip_plane(m_clip_plane);
	m_shader->upload_light_source(light);
	m_shader->upload_shadow_transforms(light);
	m_shader->upload_shadow_cascade_end(camera, light);
	m_shader->upload_view_matrix(camera.get_view_projection_matrix());
	m_shader->upload_projection_depth({ camera.get_near(), camera.get_far() });


	for (int i = 0; i < SHADOW_CASCADES; i++) {
		if (m_shadow_maps[i]) {
			m_shadow_maps[i]->bind(GL_TEXTURE1 + i);
		}
	}

	m_shader->upload_shadow_maps(1);


	GLC(glEnable(GL_DEPTH_TEST));
	GLC(glEnable(GL_CULL_FACE));
	GLC(glFrontFace(GL_CCW));
	GLC(glEnable(GL_CLIP_DISTANCE0));
	GLC(glEnable(GL_BLEND));
	GLC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


	for (const auto& model : models) {
		if (model->get_indices_count() > 0) {

			if (!intersects_frustum(model, camera)) {
				continue;
			}

			model->bind();
			model->bind_texture(GL_TEXTURE0);
			m_shader->upload_model_matrix(model->get_model_matrix());
			DRAW_CALL(GLC(glDrawElements(GL_TRIANGLES, model->get_indices_count(), GL_UNSIGNED_INT, nullptr)));
		}
	}

	GLC(glDisable(GL_CULL_FACE));
	GLC(glDisable(GL_DEPTH_TEST))
	GLC(glDisable(GL_BLEND));
	GLC(glDisable(GL_CLIP_DISTANCE0));
}

void ChunkRenderer::render_depth(const std::vector<std::shared_ptr<ChunkModel>>& models, Camera& camera, const std::shared_ptr<Light>& light, int cascade) {
	if (models.empty()) {
		return;
	}

	m_depth_shader->bind();

	m_depth_shader->upload_tex_units({ 0 });
	m_depth_shader->upload_view_projection_matrix(light->get_transform_matrix(cascade));
	m_depth_shader->upload_clip_plane(m_clip_plane);


	GLC(glEnable(GL_DEPTH_TEST));
	GLC(glEnable(GL_CULL_FACE));
	GLC(glFrontFace(GL_CCW));
	GLC(glEnable(GL_BLEND));
	GLC(glEnable(GL_CLIP_DISTANCE0));

	for (const auto& model : models) {
		if (model->get_indices_count() > 0) {

			/*if (!intersects_frustum(model, camera)) {
				continue;
			}*/

			model->bind();
			model->bind_texture(GL_TEXTURE0);
			m_depth_shader->upload_model_matrix(model->get_model_matrix());
			DRAW_CALL(GLC(glDrawElements(GL_TRIANGLES, model->get_indices_count(), GL_UNSIGNED_INT, nullptr)));
		}
	}

	GLC(glDisable(GL_BLEND));
	GLC(glDisable(GL_CULL_FACE));
	GLC(glDisable(GL_DEPTH_TEST));
	GLC(glDisable(GL_CLIP_DISTANCE0));
}