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
	m_lowpass_x_shader = std::make_unique<LowpassXShader>();

	m_lowpass_x_shader->bind();
	m_lowpass_x_shader->upload_kernel(generate_lowpass_kernel());
	m_lowpass_x_shader->upload_tex_unit(0);
}

void PostFXRenderer::render(const std::shared_ptr<FrameBuffer>& input_fbo) {
	
	/*for (int i = 0; i < 20; i++) {
		lowpass_x(input_fbo, m_pingpong_fbo);
		lowpass_x(m_pingpong_fbo, input_fbo);
	}

	unbind_fbo();

	m_plain_shader->bind();
	m_plain_shader->upload_tex_unit(0);*/

	input_fbo->get_texture()->bind(GL_TEXTURE0);

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