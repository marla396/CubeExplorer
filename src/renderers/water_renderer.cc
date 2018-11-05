#include "renderers/water_renderer.h"
#include "resources.h"


WaterRenderer::WaterRenderer(const std::shared_ptr<World>& world)
	: m_generator(std::random_device{}()), m_time(0.0f), m_L(1250), m_amplitude(2.0f), m_wind_speed(50.0f),
	m_wind_direction({ 1.0f, 1.0f }), m_capillar_supress_factor(0.1f), m_wave_strength(1.0f), m_wireframe(false),
	m_below_water(false){

	auto height_map = world->get_height_map();
	uint8_t *buffer = height_map->to_texture_buffer();

	m_height_map = std::make_unique<MTexture<uint8_t>>(WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE, buffer);

	delete[] buffer;

	initialize_h0k();
	initialize_hkt();
	initialize_twiddle();
	initialize_butterfly();
	initialize_inversion();

	m_reflection_fbo = std::make_shared<FrameBuffer>(Application::get_width() / 2, Application::get_height() / 2, FBO_TEXTURE | FBO_RENDERBUFFER);
	m_refraction_fbo = std::make_shared<FrameBuffer>(Application::get_width() / 2, Application::get_height() / 2, FBO_TEXTURE | FBO_RENDERBUFFER);

	Application::register_resize_callback([this](size_t w, size_t h) {
		m_reflection_fbo->set_resolution(w / 2, h / 2);
		m_refraction_fbo->set_resolution(w / 2, h / 2);
	});

	compute_h0k();
	compute_twiddle();
}

void WaterRenderer::set_L(int L) {
	m_L = L;
}
void WaterRenderer::set_amplitude(float amplitude) {
	m_amplitude = amplitude;
}

void WaterRenderer::set_wind_speed(float wind_speed) {
	m_wind_speed = wind_speed;
}

void WaterRenderer::set_wind_direction(const glm::vec2& wind_direction) {
	m_wind_direction = wind_direction;
}

void WaterRenderer::set_capillar_supress_factor(float factor) {
	m_capillar_supress_factor = factor;
}

void WaterRenderer::set_wave_strength(float strength) {
	m_wave_strength = strength;
}

float WaterRenderer::get_wave_strength() const {
	return m_wave_strength;
}

void WaterRenderer::toggle_wireframe() {
	m_wireframe = !m_wireframe;
}

void WaterRenderer::set_reflection_render(const std::vector<std::pair<std::function<void(Camera&)>, std::function<void(const glm::vec4&)>>>& renderers) {
	m_terrain_renderers = renderers;
}

TexStoragePtr WaterRenderer::get_tilde_h0k() const {
	return m_tilde_h0k;
}

TexStoragePtr WaterRenderer::get_tilde_h0minusk() const {
	return m_tilde_h0minusk;
}

TexStoragePtr WaterRenderer::get_tilde_hkt_dx() const {
	return m_dx;
}

TexStoragePtr WaterRenderer::get_tilde_hkt_dy() const {
	return m_dy;
}

TexStoragePtr WaterRenderer::get_tilde_hkt_dz() const {
	return m_dz;
}

TexStorageLog2Ptr WaterRenderer::get_twiddle_factors() const {
	return m_twiddle_factors;
}

TexStoragePtr WaterRenderer::get_pingpong() const {
	return m_pingpong;
}

TexStoragePtr WaterRenderer::get_normal_map() const {
	return m_normal_map;
}

std::shared_ptr<ITexture> WaterRenderer::get_reflection() const {
	return m_reflection_fbo->get_texture();
}

std::shared_ptr<ITexture> WaterRenderer::get_refraction() const {
	return m_refraction_fbo->get_texture();
}

void WaterRenderer::render(const std::vector<std::shared_ptr<Quad2DModel>>& models, Camera& camera) {
	if (models.empty()) {
		return;
	}

	render_refraction(camera);
	render_reflection(camera);

	m_shader->bind();

	m_shader->upload_tex_units({ 0, 1, 2, 3, 4, 5, 6 });
	m_shader->upload_camera_position(camera.get_position());
	m_shader->upload_displacement_factor(m_wave_strength);
	m_shader->upload_water_height(WORLD_WATER_HEIGHT / static_cast<float>(WORLD_MAX_HEIGHT));
	m_shader->upload_light_position({ WORLD_SIZE * CHUNK_SIZE * 100.0f, WORLD_WATER_HEIGHT * 2.0f, WORLD_SIZE * CHUNK_SIZE * 100.0f });


	m_shader->upload_view_matrix(camera.get_view_matrix());
	m_shader->upload_projection_matrix(camera.get_projection_matrix());

	m_dy->bind(GL_TEXTURE0);
	m_height_map->bind(GL_TEXTURE1);
	m_reflection_fbo->get_texture()->bind(GL_TEXTURE2);
	m_refraction_fbo->get_texture()->bind(GL_TEXTURE3);
	m_normal_map->bind(GL_TEXTURE4);
	m_dx->bind(GL_TEXTURE5);
	m_dz->bind(GL_TEXTURE6);

	GLC(glPatchParameteri(GL_PATCH_VERTICES, 3));
	GLC(glEnable(GL_DEPTH_TEST));
	GLC(glEnable(GL_BLEND));
	GLC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	if (m_wireframe) {
		GLC(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	} else {
		GLC(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	}

	for (const auto& model : models) {
		model->bind();
		m_shader->upload_model_matrix(model->get_model_matrix());
		DRAW_CALL(GLC(glDrawElements(GL_PATCHES, model->get_indices_count(), GL_UNSIGNED_INT, nullptr)));
	}

	if (m_wireframe)
		GLC(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		

	GLC(glDisable(GL_DEPTH_TEST));
	GLC(glDisable(GL_BLEND));
}

void WaterRenderer::update(Camera& camera, float time) {
	m_time = 3.0f * time;

	m_below_water = camera.get_position().y < WORLD_WATER_HEIGHT;

	compute_hkt();
	compute_fft();
}


void WaterRenderer::compute_h0k() const {
	m_h0k_shader->bind();
	m_h0k_shader->upload_tex_units({ 0, 1, 2, 3, 4, 5 });

	m_h0k_shader->upload_N(WATER_FFT_DIMENSION);
	m_h0k_shader->upload_L(m_L);
	m_h0k_shader->upload_amplitude(m_amplitude);
	m_h0k_shader->upload_wind_speed(m_wind_speed);
	m_h0k_shader->upload_wind_direction(m_wind_direction);
	m_h0k_shader->upload_capillar_supress_factor(m_capillar_supress_factor);

	m_tilde_h0k->bind_image_texture(0, GL_WRITE_ONLY, GL_RGBA32F);
	m_tilde_h0minusk->bind_image_texture(1, GL_WRITE_ONLY, GL_RGBA32F);

	m_noise_r0->bind(GL_TEXTURE2);
	m_noise_i0->bind(GL_TEXTURE3);
	m_noise_r1->bind(GL_TEXTURE4);
	m_noise_i1->bind(GL_TEXTURE5);

	GLC(glDispatchCompute(WATER_FFT_DIMENSION / 16, WATER_FFT_DIMENSION / 16, 1));
	GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

}

void WaterRenderer::compute_hkt() const {
	m_hkt_shader->bind();

	m_hkt_shader->upload_tex_units({ 0, 1, 2, 3, 4 });

	m_hkt_shader->upload_N(WATER_FFT_DIMENSION);
	m_hkt_shader->upload_L(m_L);
	m_hkt_shader->upload_time(m_time);

	m_tilde_hkt_dx->bind_image_texture(0, GL_WRITE_ONLY, GL_RGBA32F);
	m_tilde_hkt_dy->bind_image_texture(1, GL_WRITE_ONLY, GL_RGBA32F);
	m_tilde_hkt_dz->bind_image_texture(2, GL_WRITE_ONLY, GL_RGBA32F);
	m_tilde_h0k->bind_image_texture(3, GL_READ_ONLY, GL_RGBA32F);
	m_tilde_h0minusk->bind_image_texture(4, GL_READ_ONLY, GL_RGBA32F);


	GLC(glDispatchCompute(WATER_FFT_DIMENSION / 16, WATER_FFT_DIMENSION / 16, 1));
	GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
}

void WaterRenderer::compute_twiddle() const {
	m_twiddle_shader->bind();
	m_twiddle_shader->bind_buffer_base(1);
	m_twiddle_shader->upload_tex_unit(0);
	m_twiddle_shader->upload_N(WATER_FFT_DIMENSION);

	m_twiddle_factors->bind_image_texture(0, GL_WRITE_ONLY, GL_RGBA32F);

	GLC(glDispatchCompute(WATER_FFT_DIMENSION_LOG2, WATER_FFT_DIMENSION / 16, 1));
	GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

}

void WaterRenderer::compute_fft() const {

	int pingpong = 0;

	m_butterfly_shader->bind();
	m_butterfly_shader->upload_tex_units({ 0, 1, 2 });

	m_twiddle_factors->bind_image_texture(0, GL_READ_ONLY, GL_RGBA32F);
	m_tilde_hkt_dy->bind_image_texture(1, GL_READ_WRITE, GL_RGBA32F);
	m_pingpong->bind_image_texture(2, GL_READ_WRITE, GL_RGBA32F);

	run_fft(pingpong);

	m_inversion_shader->bind();
	m_inversion_shader->upload_N(WATER_FFT_DIMENSION);
	m_inversion_shader->upload_pingpong(pingpong);
	m_inversion_shader->upload_tex_units({ 0, 1, 2 });

	m_dy->bind_image_texture(0, GL_WRITE_ONLY, GL_RGBA32F);
	m_tilde_hkt_dy->bind_image_texture(1, GL_READ_WRITE, GL_RGBA32F);
	m_pingpong->bind_image_texture(2, GL_READ_WRITE, GL_RGBA32F);

	GLC(glDispatchCompute(WATER_FFT_DIMENSION / 16, WATER_FFT_DIMENSION / 16, 1));
	GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

	pingpong = 0;

	m_butterfly_shader->bind();

	m_twiddle_factors->bind_image_texture(0, GL_READ_ONLY, GL_RGBA32F);
	m_tilde_hkt_dx->bind_image_texture(1, GL_READ_WRITE, GL_RGBA32F);
	m_pingpong->bind_image_texture(2, GL_READ_WRITE, GL_RGBA32F);

	run_fft(pingpong);

	m_inversion_shader->bind();
	m_inversion_shader->upload_N(WATER_FFT_DIMENSION);
	m_inversion_shader->upload_pingpong(pingpong);
	m_inversion_shader->upload_tex_units({ 0, 1, 2 });

	m_dx->bind_image_texture(0, GL_WRITE_ONLY, GL_RGBA32F);
	m_tilde_hkt_dx->bind_image_texture(1, GL_READ_WRITE, GL_RGBA32F);
	m_pingpong->bind_image_texture(2, GL_READ_WRITE, GL_RGBA32F);

	GLC(glDispatchCompute(WATER_FFT_DIMENSION / 16, WATER_FFT_DIMENSION / 16, 1));
	GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

	pingpong = 0;

	m_butterfly_shader->bind();

	m_twiddle_factors->bind_image_texture(0, GL_READ_ONLY, GL_RGBA32F);
	m_tilde_hkt_dz->bind_image_texture(1, GL_READ_WRITE, GL_RGBA32F);
	m_pingpong->bind_image_texture(2, GL_READ_WRITE, GL_RGBA32F);

	run_fft(pingpong);

	m_inversion_shader->bind();
	m_inversion_shader->upload_N(WATER_FFT_DIMENSION);
	m_inversion_shader->upload_pingpong(pingpong);
	m_inversion_shader->upload_tex_units({ 0, 1, 2 });

	m_dz->bind_image_texture(0, GL_WRITE_ONLY, GL_RGBA32F);
	m_tilde_hkt_dz->bind_image_texture(1, GL_READ_WRITE, GL_RGBA32F);
	m_pingpong->bind_image_texture(2, GL_READ_WRITE, GL_RGBA32F);

	GLC(glDispatchCompute(WATER_FFT_DIMENSION / 16, WATER_FFT_DIMENSION / 16, 1));
	GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

	m_normal_shader->bind();
	m_normal_shader->upload_tex_units({ 0, 1 });
	m_normal_shader->upload_N(WATER_FFT_DIMENSION);
	m_normal_shader->upload_strength(0.5f);

	m_normal_map->bind_image_texture(0, GL_WRITE_ONLY, GL_RGBA32F);

	m_dy->bind(GL_TEXTURE1);

	GLC(glDispatchCompute(WATER_FFT_DIMENSION / 16, WATER_FFT_DIMENSION / 16, 1));
	GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
}

void WaterRenderer::run_fft(int& pingpong) const {
	//Horizontal FFT
	m_butterfly_shader->upload_direction(0);
	for (int i = 0; i < static_cast<int>(WATER_FFT_DIMENSION_LOG2); i++) {
		m_butterfly_shader->upload_stage(i);
		m_butterfly_shader->upload_pingpong(pingpong);

		GLC(glDispatchCompute(WATER_FFT_DIMENSION / 16, WATER_FFT_DIMENSION / 16, 1));
		GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		pingpong++;
		pingpong %= 2;
	}

	//Vertical FFT
	m_butterfly_shader->upload_direction(1);
	for (int i = 0; i < static_cast<int>(WATER_FFT_DIMENSION_LOG2); i++) {
		m_butterfly_shader->upload_stage(i);
		m_butterfly_shader->upload_pingpong(pingpong);

		GLC(glDispatchCompute(WATER_FFT_DIMENSION / 16, WATER_FFT_DIMENSION / 16, 1));
		GLC(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		pingpong++;
		pingpong %= 2;
	}
}


void WaterRenderer::render_reflection(Camera& camera) {

	bind_fbo(m_reflection_fbo);

	GLC(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	auto camera_pos = camera.get_position();
	auto camera_rot = camera.get_rotation();

	float dist = 2 * (camera_pos.y - static_cast<float>(WORLD_WATER_HEIGHT));
	camera_pos.y -= dist;
	camera_rot.x *= -1;

	camera.set_position(camera_pos);
	camera.set_rotation(camera_rot);

	for (const auto& renderer : m_terrain_renderers) {
		renderer.second({ 0.0f, 1.0f, 0.0f, -static_cast<float>(WORLD_WATER_HEIGHT) });
		renderer.first(camera);
		renderer.second({ 0.0f, 0.0, 0.0f, 0.0f });
	}

	camera_pos.y += dist;
	camera_rot.x *= -1;

	camera.set_position(camera_pos);
	camera.set_rotation(camera_rot);

	unbind_fbo();
}

void WaterRenderer::render_refraction(Camera& camera) {
	bind_fbo(m_refraction_fbo);

	GLC(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	float clip_offset = static_cast<float>(WORLD_WATER_HEIGHT);
	float clip_direction = -1.0f;

	if (m_below_water) {
		clip_offset *= -1.0f;
		clip_direction *= -1.0f;
	}

	for (const auto& renderer : m_terrain_renderers) {
		renderer.second({ 0.0f, clip_direction, 0.0f, clip_offset });
		renderer.first(camera);
		renderer.second({ 0.0f, 0.0f, 0.0f, 0.0f });
	}

	unbind_fbo();
}

void WaterRenderer::initialize_h0k() {
	m_h0k_shader = std::make_unique<H0kShader>();

	m_tilde_h0k = std::make_shared<TexStorage>();
	m_tilde_h0minusk = std::make_shared<TexStorage>();

	{
		auto noise1 = m_generator.generate<float, 4 * WATER_FFT_DIMENSION, 4 * WATER_FFT_DIMENSION>(0.0f, 1.0f);
		m_noise_r0 = std::make_unique<MTexture<float>>(WATER_FFT_DIMENSION, WATER_FFT_DIMENSION, reinterpret_cast<float*>(noise1.data()));
	}
	{
		auto noise2 = m_generator.generate<float, 4 * WATER_FFT_DIMENSION, 4 * WATER_FFT_DIMENSION>(0.0f, 1.0f);
		m_noise_i0 = std::make_unique<MTexture<float>>(WATER_FFT_DIMENSION, WATER_FFT_DIMENSION, reinterpret_cast<float*>(noise2.data()));
	}
	{
		auto noise3 = m_generator.generate<float, 4 * WATER_FFT_DIMENSION, 4 * WATER_FFT_DIMENSION>(0.0f, 1.0f);
		m_noise_r1 = std::make_unique<MTexture<float>>(WATER_FFT_DIMENSION, WATER_FFT_DIMENSION, reinterpret_cast<float*>(noise3.data()));
	}
	{
		auto noise4 = m_generator.generate<float, 4 * WATER_FFT_DIMENSION, 4 * WATER_FFT_DIMENSION>(0.0f, 1.0f);
		m_noise_i1 = std::make_unique<MTexture<float>>(WATER_FFT_DIMENSION, WATER_FFT_DIMENSION, reinterpret_cast<float*>(noise4.data()));
	}
}

void WaterRenderer::initialize_hkt() {
	m_hkt_shader = std::make_unique<HktShader>();

	m_tilde_hkt_dx = std::make_shared<TexStorage>();
	m_tilde_hkt_dy = std::make_shared<TexStorage>();
	m_tilde_hkt_dz = std::make_shared<TexStorage>();
}

void WaterRenderer::initialize_twiddle() {
	m_twiddle_shader = std::make_unique<TwiddleShader>();
	m_twiddle_factors = std::make_shared<TexStorageLog2>();
}

void WaterRenderer::initialize_butterfly() {
	m_butterfly_shader = std::make_unique<ButterflyShader>();
	m_pingpong = std::make_shared<TexStorage>();
}

void WaterRenderer::initialize_inversion() {
	m_inversion_shader = std::make_unique<InversionShader>();
	m_normal_shader = std::make_unique<NormalShader>();
	m_normal_map = std::make_shared<TexStorage>();
	m_dx = std::make_shared<TexStorage>();
	m_dy = std::make_shared<TexStorage>();
	m_dz = std::make_shared<TexStorage>();
}