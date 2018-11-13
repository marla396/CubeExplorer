#include "renderers/postfx_renderer.h"

#include <random>
#include <numeric>

PostFXRenderer::PostFXRenderer() {
	m_pingpong_fbo = std::make_shared<FrameBuffer>(Application::get_width(), Application::get_height(), FBO_TEXTURE | FBO_RENDERBUFFER);

	Application::register_resize_callback([this](size_t w, size_t h) {
		m_pingpong_fbo->set_resolution(w, h);
	});

	m_quad = std::make_unique<Quad2DModel>(1.0f);
	m_quad->set_position({ 0.0f, 0.0f, 0.0f });

	m_plain_shader = std::make_unique<PlainShader>();
	
	init_ssao();

	m_lowpass_x_shader = std::make_unique<LowpassXShader>();

	m_lowpass_x_shader->bind();
	m_lowpass_x_shader->upload_kernel(generate_lowpass_kernel());
	m_lowpass_x_shader->upload_tex_unit(0);
}

void PostFXRenderer::render(const std::shared_ptr<FrameBuffer>& input_fbo, Camera& camera) {

	bind_fbo(m_pingpong_fbo);

	GLC(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_ssao_shader->bind();
	m_ssao_shader->upload_resolution({ static_cast<float>(Application::get_width()), static_cast<float>(Application::get_height()) });
	m_ssao_shader->upload_projection_matrix(camera.get_projection_matrix());

	input_fbo->get_texture(1)->bind(GL_TEXTURE0);
	input_fbo->get_texture(2)->bind(GL_TEXTURE1);
	m_ssao_noise->bind(GL_TEXTURE2);

	m_quad->bind();
	DRAW_CALL(GLC(glDrawElements(GL_TRIANGLES, m_quad->get_indices_count(), GL_UNSIGNED_INT, nullptr)));

	unbind_fbo();

	m_plain_shader->bind();
	m_plain_shader->upload_tex_unit(0);

	input_fbo->get_texture(0)->bind(GL_TEXTURE0);
	
	m_quad->bind();
	DRAW_CALL(GLC(glDrawElements(GL_TRIANGLES, m_quad->get_indices_count(), GL_UNSIGNED_INT, nullptr)));
}

void PostFXRenderer::lowpass_x(const std::shared_ptr<FrameBuffer>& input_fbo, const std::shared_ptr<FrameBuffer>& output_fbo) {
	bind_fbo(output_fbo);

	GLC(glClear(GL_COLOR_BUFFER_BIT));

	m_lowpass_x_shader->bind();
	input_fbo->get_texture()->bind(GL_TEXTURE0);

	m_quad->bind();
	DRAW_CALL(GLC(glDrawElements(GL_TRIANGLES, m_quad->get_indices_count(), GL_UNSIGNED_INT, nullptr)));

	unbind_fbo();
}

void PostFXRenderer::init_ssao() {

	m_ssao_shader = std::make_unique<SSAOShader>();
	m_ssao_shader->upload_tex_units({ 0, 1, 2 });


	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	std::mt19937 rng(std::random_device{}());
	std::vector<glm::vec3> ssao_kernel;

	for (int i = 0; i < 64; i++) {
		glm::vec3 sample = { dist(rng) * 2.0f - 1.0f, dist(rng) * 2.0f - 1.0f, dist(rng) };
		sample = glm::normalize(sample);
		sample *= dist(rng);

		float scale = static_cast<float>(i) / 64.0f;
		scale = 0.1f + scale * scale * 0.9f;

		sample *= scale;

		ssao_kernel.push_back(sample);
	}

	m_ssao_shader->upload_kernel(ssao_kernel);

	std::vector<glm::vec3> ssao_noise;

	for (int i = 0; i < 16; i++) {
		glm::vec3 noise = { dist(rng) * 2.0f - 1.0f, dist(rng) * 2.0f - 1.0f, 0.0f };
		ssao_noise.push_back(noise);
	}

	m_ssao_noise = std::make_unique<MTexture<float>>(4, 4, reinterpret_cast<float*>(&ssao_noise[0]));
}


std::vector<float> PostFXRenderer::generate_lowpass_kernel() {
	std::vector<float> ret;

	std::mt19937 rng(std::random_device{}());
	std::normal_distribution<float> dist(0.0f, 1.0f);

	for (int i = 0; i < 21; i++) {
		ret.push_back(dist(rng));
	}

	float sum = std::accumulate(ret.begin(), ret.end(), 0.0f);

	for (auto& f : ret) {
		f /= sum;
	}

	std::sort(ret.begin(), ret.end());

	return { 0.057138f, 0.056559f, 0.054856f, 0.052132f, 0.048544f, 0.044292f, 0.039597f, 0.034685f, 0.02977f, 0.025037f, 0.020631f, 0.016658f, 0.013178f, 0.010216f, 0.007759f, 0.005774f, 0.004211f, 0.003009f, 0.002106f, 0.001445f, 0.000971f };
}

