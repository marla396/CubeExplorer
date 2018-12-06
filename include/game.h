#pragma once

#include <memory>
#include <map>

#include "camera.h"
#include "hud/hud_renderer.h"
#include "renderers/chunk_renderer.h"
#include "renderers/water_renderer.h"
#include "renderers/skybox_renderer.h"
#include "renderers/postfx_renderer.h"
#include "renderers/entity_renderer.h"
#include "texture/texture_atlas.h"
#include "world/world.h"
#include "entities/player.h"
#include "framebuffer.h"

class Game {
public:
	Game(NVGcontext* nvg_ctx);

	void on_render();
	void on_update(float time, float dt);
	void on_resize(size_t width, size_t height);
	void on_cursor(float dx, float dy);
	void on_key(int key, int scan_code, int action, int mods);
private:
	void render_shadow_maps();
	void insert_display_pip();

	enum PictureInPicture {
		HEIGHT_MAP = 1,
		H0K = 2,
		H0KMINUS = 4,
		TWIDDLE = 8,
		HKT = 16,
		DY = 32,
		NORMAL = 64,
		DUDV = 128,
		REFRACTION = 256,
		REFLECTION = 512,
		SHADOW_MAPS = 1024,
		SSAO = 2048,
	};

	NVGcontext* m_nvg_ctx;

	bool m_show_pip;
	uint32_t m_pip;
	bool m_free_cam;
	bool m_show_hud;
	bool m_hidden_cursor;

	Camera m_camera;

	std::shared_ptr<World> m_world;

	std::shared_ptr<FrameBuffer> m_postfx_fbo;
	std::shared_ptr<FrameBuffer> m_shadow_fbo;

	std::unique_ptr<ChunkRenderer> m_chunk_renderer;
	std::unique_ptr<EntityRenderer> m_entity_renderer;
	std::unique_ptr<WaterRenderer> m_water_renderer;
	std::unique_ptr<HUDRenderer> m_hud_renderer;
	std::unique_ptr<SkyboxRenderer> m_skybox_renderer;
	std::unique_ptr<PostFXRenderer> m_postfx_renderer;

	std::shared_ptr<TextureAtlas<32, 32>> m_texture_atlas;

	std::shared_ptr<HUDTexture> m_height_map_texture;
	std::map<std::string, std::shared_ptr<HUDTexture>> m_hud_textures;
};