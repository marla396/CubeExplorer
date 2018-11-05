#include <chrono>
#include <cmath>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

#include "game.h"
#include "gl.h"
#include "models/block_model.h"
#include "renderers/block_renderer.h"
#include "texture/texture_atlas.h"
#include "texture/ftexture.h"
#include "texture/mtexture.h"
#include "models/chunk_model.h"
#include "renderers/chunk_renderer.h"
#include "misc/fft_noise_generator.h"
#include "renderers/renderer.h"

#include "shaders/invert_shader.h"

std::shared_ptr<TextureAtlas<32, 32>> texture_atlas;
std::shared_ptr<Quad2DModel> water_model;
std::shared_ptr<SkyboxModel> skybox_model;
std::shared_ptr<FTexture> skybox_texture;

Game::Game(NVGcontext* nvg_ctx) 
	: m_nvg_ctx(nvg_ctx), m_show_height_map(false), m_free_cam(true), m_show_hud(true), m_hidden_cursor(true){
	m_camera.set_position({ 0.0f, 0.0f, 0.0f });

	m_camera.set_near(0.01f);
	m_camera.set_far(1024.0f);
	m_camera.set_aspect(1.0f);
	m_camera.set_fov(PI / 4.0f);

	m_camera.set_position({ 0.0f, WORLD_MAX_HEIGHT, 0.0f });
	m_camera.set_yaw(PI);

	m_postfx_fbo = std::make_shared<FrameBuffer>(Application::get_width(), Application::get_height(), FBO_TEXTURE | FBO_DEPTH_TEXTURE | FBO_RENDERBUFFER);

	Application::register_resize_callback([this](size_t w, size_t h){
		m_postfx_fbo->set_resolution(w, h);
	});

	texture_atlas = std::make_shared<TextureAtlas<32, 32>>("blocktextures.png");
	skybox_texture = std::make_shared<FTexture>("skybox.png");
	 
	//auto seed = std::random_device{}();
	auto seed = 1337;

	std::cout << "Seed: " << seed << std::endl;

	m_world = std::make_shared<World>(seed);
	m_world->generate_world(texture_atlas);

	auto height_map = m_world->get_height_map();
	
	uint8_t* buffer = height_map->to_texture_buffer();

	m_height_map_texture = std::make_shared<HUDTexture>(std::make_shared<MTexture<uint8_t>>(WORLD_SIZE * CHUNK_SIZE, WORLD_SIZE * CHUNK_SIZE, buffer));
	m_height_map_texture->set_position({ 0.6f, 0.6f });
	m_height_map_texture->set_size({ 0.4f, 0.4f });

	water_model = std::make_shared<Quad2DModel>();
	water_model->set_position({ -0.5f, WORLD_WATER_HEIGHT, -0.5f });
	water_model->set_scale({ static_cast<float>(WORLD_SIZE * CHUNK_SIZE), 1.0f, static_cast<float>(WORLD_SIZE * CHUNK_SIZE)});

	skybox_model = std::make_shared<SkyboxModel>(skybox_texture);

	m_chunk_renderer = std::make_unique<ChunkRenderer>();
	m_water_renderer = std::make_unique<WaterRenderer>(m_world);
	m_hud_renderer = std::make_unique<HUDRenderer>(m_nvg_ctx);
	m_skybox_renderer = std::make_unique<SkyboxRenderer>();
	m_postfx_renderer = std::make_unique<PostFXRenderer>();

}

void Game::on_render() {

	using namespace std::placeholders;

	//bind_fbo(m_postfx_fbo);

	GLC(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));


	std::vector<std::shared_ptr<ChunkModel>> chunks;

	for (const auto& c : *m_world->get_chunks()) {
		chunks.push_back(c.second);
	}

	m_skybox_renderer->render({ skybox_model }, m_camera);


	m_chunk_renderer->render(chunks, m_camera);

	std::function<void(Camera&)> a = std::bind(&ChunkRenderer::render, m_chunk_renderer.get(), chunks, _1);
	std::function<void(const glm::vec4&)> b = std::bind(&ChunkRenderer::set_clip_plane, m_chunk_renderer.get(), _1);

	std::vector<std::shared_ptr<SkyboxModel>> sky_boxes = { skybox_model };

	std::function<void(Camera&)> c = std::bind(&SkyboxRenderer::render, m_skybox_renderer.get(), sky_boxes, _1);
	std::function<void(const glm::vec4&)> d = std::bind(&SkyboxRenderer::set_clip_plane, m_skybox_renderer.get(), _1);


	m_water_renderer->set_reflection_render({ std::make_pair(c, d), std::make_pair(a, b) });
	m_water_renderer->render({ water_model }, m_camera);
	
	//m_postfx_renderer->render(m_postfx_fbo);

	m_hud_textures.clear();

	std::shared_ptr<HUDTexture> hud = std::make_shared<HUDTexture>(m_water_renderer->get_refraction());
	hud->set_position({ 0.6f, 0.6f });
	hud->set_size({ 0.4f, 0.4f });
	m_hud_textures.insert(std::make_pair("ComputeShader", hud));
	
	if (m_show_hud)
		m_hud_renderer->render(m_hud_textures, m_camera, m_player);
}

void Game::on_update(float time, float dt) {
	UNUSED(time);

	m_water_renderer->update(m_camera, time);

	if (m_free_cam)
		m_camera.process_keyboard(dt);
	else
		m_player.update(m_world, m_camera, dt);
	

	if (Application::key_down(GLFW_KEY_PAGE_UP)) {
		auto strength = m_water_renderer->get_wave_strength();
		m_water_renderer->set_wave_strength(strength + 0.05f);
	}

	if (Application::key_down(GLFW_KEY_PAGE_DOWN)) {
		auto strength = m_water_renderer->get_wave_strength();
		m_water_renderer->set_wave_strength(strength - 0.05f);
	}

	if (Application::key_down(GLFW_KEY_MINUS)) {
		Application::add_time_warp_factor(0.1f);
	}

	if (Application::key_down(GLFW_KEY_SLASH)) {
		Application::add_time_warp_factor(-0.1f);
	}

	if (Application::key_down(GLFW_KEY_PAGE_DOWN)) {
		auto strength = m_water_renderer->get_wave_strength();
		m_water_renderer->set_wave_strength(strength - 0.05f);
	}
}

void Game::on_resize(size_t width, size_t height) {
	float w = static_cast<float>(width);
	float h = static_cast<float>(height);

	m_camera.set_fov(std::atan2(w, h));
	m_camera.set_aspect(w / h);
}

void Game::on_cursor(float dx, float dy) {
	m_camera.process_mouse(dx, dy);
}

void Game::on_key(int key, int scan_code, int action, int mods) {
	
	UNUSED(scan_code);
	UNUSED(mods);

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_F1) {
			m_show_height_map = !m_show_height_map;

			if (m_show_height_map) {
				m_hud_textures.insert(std::make_pair("height_map_hud", m_height_map_texture));
			} else {
				m_hud_textures.erase("height_map_hud");
			}
		}

		if (key == GLFW_KEY_F2) {
			m_free_cam = !m_free_cam;

			m_player.set_position(m_camera.get_position());
		}

		if (key == GLFW_KEY_F3) {
			m_show_hud = !m_show_hud;
		}

		if (key == GLFW_KEY_ESCAPE) {
			Application::exit();
			return;
		}

		if (key == GLFW_KEY_F11) {
			m_water_renderer->toggle_wireframe();
		}
	}
}

