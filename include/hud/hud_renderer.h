#pragma once

#include <sstream>
#include <map>

#include "application.h"
#include "../shaders/hud_shader.h"
#include "hud_texture.h"
#include "camera.h"
#include "entities/player.h"

class HUDRenderer{
public:
    HUDRenderer(NVGcontext* nvg_ctx);

    void render(const std::map<std::string, std::shared_ptr<HUDTexture>>& hud_textures, const Camera& camera, const Player& player);
private:
    void begin_frame();
    void end_frame();

    void render_text(const std::string& text, float x, float y);
	void render_shadow_text(const std::string& text, float x, float y, float r, float g, float b, float a);

    void set_font(const std::string& font_name);
    void set_font_size(float font_size);
    void set_text_align(int align);
	void set_fill_color(float r, float g, float b, float a);
	void set_stroke_color(float r, float g, float b, float a);
	void set_stroke_width(float width);

	std::unique_ptr<HUDShader> m_shader;
    NVGcontext* m_nvg_ctx;
};