#include "renderers/postfx_renderer.h"
#include "world/world_const.h"

#include <random>
#include <numeric>

PostFXRenderer::PostFXRenderer() : m_fxaa(true), m_ssao(false) {
	m_ssao_fbo = std::make_shared<FrameBuffer>(Application::get_width() / SSAO_SCALE, Application::get_height() / SSAO_SCALE, FBO_TEXTURE | FBO_RENDERBUFFER);

	m_ssao_pingpong_texture = std::make_shared<MTexture<float>>(Application::get_width() / SSAO_SCALE, Application::get_height() / SSAO_SCALE, static_cast<float*>(nullptr));
	m_pingpong_texture = std::make_shared<MTexture<float>>(Application::get_width(), Application::get_height(), static_cast<float*>(nullptr));
	

	Application::register_resize_callback([this](size_t w, size_t h) {
		m_ssao_fbo->set_resolution(w / SSAO_SCALE, h / SSAO_SCALE);
		m_ssao_pingpong_texture = std::make_shared<MTexture<float>>(w / SSAO_SCALE, h / SSAO_SCALE, static_cast<float*>(nullptr));
		m_pingpong_texture = std::make_shared<MTexture<float>>(w, h, static_cast<float*>(nullptr));
	});

	m_quad = std::make_unique<Quad2DModel>(1.0f);
	m_quad->set_position({ 0.0f, 0.0f, 0.0f });

	m_plain_shader = std::make_unique<PlainShader>();
	
	m_underwater_shader = std::make_unique<UnderwaterShader>();
	m_underwater_shader->upload_tex_units({ 0, 1 });

	m_underwater_texture = std::make_unique<FTexture>("underwater.png", ITexture::RGBA32F);

	init_ssao();

	m_fxaa_shader = std::make_unique<FXAAShader>();
	m_fxaa_shader->upload_tex_units({ 0, 1 });
}

void PostFXRenderer::render(const std::shared_ptr<FrameBuffer>& input_fbo, Camera& camera) {

	std::shared_ptr<ITexture> tex1 = input_fbo->get_texture();
	std::shared_ptr<ITexture> tex2 = m_pingpong_texture;

	if (m_ssao) {

		bind_fbo(m_ssao_fbo);

		GLC(glClear(GL_COLOR_BUFFER_BIT));

		m_ssao_shader->bind();
		m_ssao_shader->upload_projection_depth({ camera.get_near(), camera.get_far() });
		m_ssao_shader->upload_screen_dimensions(glm::vec2{ static_cast<float>(Application::get_width()), static_cast<float>(Application::get_height()) });

		input_fbo->get_depth_texture()->bind(GL_TEXTURE0);
		m_ssao_noise->bind(GL_TEXTURE1);

		m_quad->bind();
		DRAW_CALL(GLC(glDrawElements(GL_TRIANGLES, m_quad->get_indices_count(), GL_UNSIGNED_INT, nullptr)));

		unbind_fbo();
		
		auto work = get_work_groups(SSAO_SCALE);

		m_ssao_blur_shader->bind();
		m_ssao_fbo->get_texture()->bind(GL_TEXTURE0);
		m_ssao_pingpong_texture->bind_image_texture(1, GL_WRITE_ONLY, GL_RGBA32F);
		
		GLC(glDispatchCompute(work.x, work.y, work.z));
		GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		m_ssao_merge_shader->bind();

		input_fbo->get_texture()->bind_image_texture(0, GL_READ_ONLY, GL_RGBA32F);
		m_ssao_pingpong_texture->bind_image_texture(1, GL_READ_ONLY, GL_RGBA32F);
		m_pingpong_texture->bind_image_texture(2, GL_WRITE_ONLY, GL_RGBA32F);


		work = get_work_groups();

		GLC(glDispatchCompute(work.x, work.y, work.z));
		GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
		
		tex1 = m_pingpong_texture;
		tex2 = input_fbo->get_texture();
	}

	float depth = WORLD_WATER_HEIGHT - camera.get_position().y;
	if (depth > 0.0f) {
		underwater(tex1, tex2, depth);
		tex1 = tex2;
	}

	if (m_fxaa){
		fxaa(tex1, tex2);
		tex1 = tex2;
	}

	m_plain_shader->bind();
	m_plain_shader->upload_tex_unit(0);

	tex1->bind(GL_TEXTURE0);
	
	m_quad->bind();
	DRAW_CALL(GLC(glDrawElements(GL_TRIANGLES, m_quad->get_indices_count(), GL_UNSIGNED_INT, nullptr)));
}

void PostFXRenderer::update(float time) {
	m_time = time;
}

void PostFXRenderer::toggle_fxaa(){
	m_fxaa = !m_fxaa;
}

void PostFXRenderer::toggle_ssao() {	
	m_ssao = !m_ssao;
}

std::shared_ptr<ITexture> PostFXRenderer::get_ssao_texture() const {
	return m_ssao_fbo->get_texture();
}

void PostFXRenderer::underwater(const std::shared_ptr<ITexture>& input, const std::shared_ptr<ITexture>& output, float depth) {

	m_underwater_shader->bind();
	m_underwater_shader->upload_time(m_time);
	m_underwater_shader->upload_depth(depth);

	input->bind_image_texture(0, GL_READ_ONLY, GL_RGBA32F);
	output->bind_image_texture(1, GL_WRITE_ONLY, GL_RGBA32F);

	auto work = get_work_groups();

	GLC(glDispatchCompute(work.x, work.y, work.z));
	GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
}

void PostFXRenderer::fxaa(const std::shared_ptr<ITexture>& input, const std::shared_ptr<ITexture>& output){

	m_fxaa_shader->bind();
	m_fxaa_shader->upload_threshold(1.0f/16.0f);

	input->bind_image_texture(0, GL_READ_ONLY, GL_RGBA32F);
	output->bind_image_texture(1, GL_WRITE_ONLY, GL_RGBA32F);

	auto work = get_work_groups();

	GLC(glDispatchCompute(work.x, work.y, work.z));
	GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
}

void PostFXRenderer::init_ssao() {

	m_ssao_shader = std::make_unique<SSAOShader>();
	m_ssao_shader->upload_tex_units({ 0, 1 });


	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	std::mt19937 rng(std::random_device{}());
	std::vector<glm::vec3> ssao_kernel;


	const float KERNEL_SIZE = 64.0f;

	for (float i = 0.0f; i < KERNEL_SIZE; i += 1.0f) {
		glm::vec3 sample = { dist(rng) * 2.0f - 1.0f, dist(rng) * 2.0f - 1.0f, dist(rng) };
		sample = glm::normalize(sample);
		sample *= dist(rng);

		float scale = i / KERNEL_SIZE;
		scale = 0.1f + scale * scale * 0.9f;

		sample *= scale;

		ssao_kernel.push_back(sample);
	}

	m_ssao_shader->upload_kernel(ssao_kernel);

	std::vector<float> ssao_noise;

	for (int i = 0; i < 16; i++) {
		ssao_noise.push_back(dist(rng) * 2.0f - 1.0f);
		ssao_noise.push_back(dist(rng) * 2.0f - 1.0f);
		ssao_noise.push_back(dist(rng) * 2.0f - 1.0f);
		ssao_noise.push_back(0.0f);
	}

	m_ssao_noise = std::make_unique<MTexture<float>>(4, 4, &ssao_noise[0]);
	m_ssao_noise->set_wrap(GL_REPEAT);

	m_ssao_merge_shader = std::make_unique<SSAOMergeShader>();
	m_ssao_merge_shader->upload_tex_units({ 0, 1, 2 });

	m_ssao_blur_shader = std::make_unique<SSAOBlurShader>();

	m_ssao_blur_shader->bind();
	m_ssao_blur_shader->upload_tex_units({ 0, 1 });


}


std::vector<float> PostFXRenderer::generate_lowpass_kernel() {
	return { 0.057138f, 0.056559f, 0.054856f, 0.052132f, 0.048544f, 0.044292f, 0.039597f, 0.034685f, 0.02977f, 0.025037f, 0.020631f, 0.016658f, 0.013178f, 0.010216f, 0.007759f, 0.005774f, 0.004211f, 0.003009f, 0.002106f, 0.001445f, 0.000971f };
}

glm::ivec3 PostFXRenderer::get_work_groups(int scale) const {
	
	int w = static_cast<int>(Application::get_width()) / scale;
	int h = static_cast<int>(Application::get_height()) / scale;

	int work_x = (w + (16 - (w % 16))) / 16;
	int work_y = (h + (16 - (h % 16))) / 16;
	int work_z = 1;

	return { work_x, work_y, work_z };
}
