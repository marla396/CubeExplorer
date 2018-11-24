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
#include "models/water_model.h"
#include "misc/fft_noise_generator.h"
#include "renderers/renderer.h"

std::shared_ptr<TextureAtlas<32, 32>> texture_atlas;
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
	m_camera.set_yaw((3.0f * PI) / 4.0f);

	m_postfx_fbo = std::make_shared<FrameBuffer>(Application::get_width(), Application::get_height(), FBO_TEXTURE | FBO_DEPTH_TEXTURE | FBO_RENDERBUFFER);
	m_shadow_fbo = std::make_shared<FrameBuffer>(Application::get_width(), Application::get_height(), FBO_DEPTH_TEXTURE | FBO_DEPTH_TEXTURE2);

	Application::register_resize_callback([this](size_t w, size_t h){
		m_postfx_fbo->set_resolution(w, h);
		m_shadow_fbo->set_resolution(8 * w, 8 * h);
	});

	texture_atlas = std::make_shared<TextureAtlas<32, 32>>("christmastextures.png");
	skybox_texture = std::make_shared<FTexture>("skybox.png");
	 
	auto seed = std::random_device{}();
	//auto seed = 1337;

	std::cout << "Seed: " << seed << std::endl;

	m_world = std::make_shared<World>(seed);
	m_world->generate_world(texture_atlas);

	m_height_map_texture = std::make_shared<HUDTexture>(m_world->get_height_map_texture());
	m_height_map_texture->set_position({ 0.6f, 0.6f });
	m_height_map_texture->set_size({ 0.4f, 0.4f });



	skybox_model = std::make_shared<SkyboxModel>(skybox_texture);

	m_chunk_renderer = std::make_unique<ChunkRenderer>();
	m_entity_renderer = std::make_unique<EntityRenderer>();
	m_water_renderer = std::make_unique<WaterRenderer>(m_world);
	m_hud_renderer = std::make_unique<HUDRenderer>(m_nvg_ctx);
	m_skybox_renderer = std::make_unique<SkyboxRenderer>();
	m_postfx_renderer = std::make_unique<PostFXRenderer>();

}

void Game::on_render() {

	GLC(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

	bind_fbo(m_postfx_fbo);
	m_postfx_fbo->bind_depth_texture(0);

	GLC(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_world->lock();

	render_shadow_maps();

	auto chunks = m_world->get_chunks();
	auto entities = m_world->get_entities();
	auto trees = m_world->get_trees();
	auto water_models = m_world->get_water_models();

	m_skybox_renderer->render({ skybox_model }, m_camera, m_world->get_sun());
	m_chunk_renderer->render(*chunks, m_camera, m_world->get_sun());
	m_entity_renderer->render(*entities, m_camera, m_world->get_sun());
	m_water_renderer->render(*water_models, m_camera, m_world->get_sun());
	m_chunk_renderer->render(*trees, m_camera, m_world->get_sun());
	
	m_world->unlock();

	unbind_fbo();

	m_postfx_renderer->render(m_postfx_fbo, m_camera);

	m_hud_textures.clear();

	std::shared_ptr<HUDTexture> hud = std::make_shared<HUDTexture>(m_postfx_fbo->get_depth_texture());
	hud->set_position({ 0.6f, 0.6f });
	hud->set_size({ 0.4f, 0.4f });
	std::shared_ptr<HUDTexture> hud2 = std::make_shared<HUDTexture>(m_shadow_fbo->get_depth_texture(0));
	hud2->set_position({ 0.6f, -0.6f });
	hud2->set_size({ 0.4f, 0.4f });
	m_hud_textures.insert(std::make_pair("ComputeShader", hud));
	//m_hud_textures.insert(std::make_pair("ComputeShadexr", hud2));
	
	if (m_show_hud)
		m_hud_renderer->render(m_hud_textures, m_camera, m_world->get_player());
}

void Game::on_update(float time, float dt) {
	using namespace std::placeholders;
	
	m_world->lock();

	m_world->update(time, m_camera);

	if (m_free_cam)
		m_camera.process_keyboard(dt);
	else
		m_world->get_player()->update(m_world, m_camera, dt);

	auto chunks = m_world->get_chunks();
	auto entities = m_world->get_entities();
	auto trees = m_world->get_trees();

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

	std::vector<std::pair<std::function<void(Camera&)>, std::function<void(const glm::vec4&)>>> render_delegates;

	render_delegates.push_back(m_skybox_renderer->get_render_delegate({ skybox_model }, m_world->get_sun()));
	render_delegates.push_back(m_chunk_renderer->get_render_delegate(*chunks, m_world->get_sun()));
	render_delegates.push_back(m_entity_renderer->get_render_delegate(*entities, m_world->get_sun()));
	render_delegates.push_back(m_chunk_renderer->get_render_delegate(*trees, m_world->get_sun()));

	m_water_renderer->set_terrain_renderers(render_delegates);

	m_water_renderer->update(m_camera, time);
	m_postfx_renderer->update(time);

	m_world->unlock();
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

			if (!m_free_cam)
				m_world->get_player()->set_position(m_camera.get_position());
		}

		if (key == GLFW_KEY_F3) {
			m_show_hud = !m_show_hud;
		}

		if (key == GLFW_KEY_F4) {
			m_camera.set_position(m_world->get_sun()->get_position());
		}

		if (key == GLFW_KEY_F5) {
			m_camera.toggle_lock_frustum();
		}

		if (key == GLFW_KEY_F6) {
			m_world->lock();
			m_world->clear_world();
			m_world->set_seed(std::random_device{}());
			m_world->generate_world(texture_atlas);
			m_world->unlock();
		}

		if (key == GLFW_KEY_F7){
			m_postfx_renderer->toggle_fxaa();
		}

		if (key == GLFW_KEY_F8) {
			m_postfx_renderer->toggle_ssao();
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

void Game::render_shadow_maps() {
	bind_fbo(m_shadow_fbo);

	GLC(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));

	m_shadow_fbo->bind_depth_texture(0);

	GLC(glClear(GL_DEPTH_BUFFER_BIT));

	m_chunk_renderer->render_depth(*m_world->get_chunks(), m_camera, m_world->get_sun(), Light::LOW);
	m_entity_renderer->render_depth(*m_world->get_entities(), m_camera, m_world->get_sun(), Light::LOW);
	m_chunk_renderer->render_depth(*m_world->get_trees(), m_camera, m_world->get_sun(), Light::LOW);

	m_shadow_fbo->bind_depth_texture(1);

	GLC(glClear(GL_DEPTH_BUFFER_BIT));

	m_chunk_renderer->render_depth(*m_world->get_chunks(), m_camera, m_world->get_sun(), Light::HIGH);
	m_entity_renderer->render_depth(*m_world->get_entities(), m_camera, m_world->get_sun(), Light::HIGH);

	GLC(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

	unbind_fbo();

	m_chunk_renderer->set_shadow_map_low(m_shadow_fbo->get_depth_texture(0));
	m_entity_renderer->set_shadow_map_low(m_shadow_fbo->get_depth_texture(0));
	m_water_renderer->set_shadow_map_low(m_shadow_fbo->get_depth_texture(0));

	m_chunk_renderer->set_shadow_map_high(m_shadow_fbo->get_depth_texture(1));
	m_entity_renderer->set_shadow_map_high(m_shadow_fbo->get_depth_texture(1));
	m_water_renderer->set_shadow_map_high(m_shadow_fbo->get_depth_texture(1));
}