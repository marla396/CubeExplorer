#include <nanovg/nanovg.h>
#include <iomanip>

#include "hud/hud_renderer.h"
#include "application.h"
#include "resources.h"

HUDRenderer::HUDRenderer(NVGcontext* nvg_ctx) : m_nvg_ctx(nvg_ctx) {
    auto res = nvgCreateFont(m_nvg_ctx, "main", Resources::resolve_path("res/fonts/FFFFORWA.ttf").c_str());
    if (res == -1){
        LOG("Failed to load font FFFFORWA.ttf" << std::endl);
    }

	m_shader = std::make_unique<HUDShader>();
}

void HUDRenderer::render(const std::map<std::string, std::shared_ptr<HUDTexture>>& hud_textures, const Camera& camera, const std::shared_ptr<Player>& player){

	bool m_height_map = false;

	if (!hud_textures.empty()) {
		m_shader->bind();
		for (const auto& h : hud_textures) {
			m_shader->upload_transformation_matrix(h.second->get_transformation_matrix());
			h.second->render();

			if (h.first == "height_map_hud") {
				m_height_map = true;
				
			}
		}
	}

    begin_frame();

	set_fill_color(1.0f, 1.0f, 1.0f, 1.0f);
    set_font_size(18.0f);
    set_text_align(NVG_ALIGN_LEFT);

    std::ostringstream log;
	log << std::fixed << std::setprecision(2);
    log << "Render time: " << Application::get_render_time() << " ms";
    render_shadow_text(log.str(), 10.0f, 20.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    
    log.str("");
    log.clear();

	static float previous_fps = 0.0f;

	float fps = 0.9f * previous_fps + 0.1f * Application::get_fps();
	if (fps != INFINITY)
		previous_fps = fps;
    log << "FPS: " << fps;
    render_shadow_text(log.str(), 10.0f, 45.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	log.str("");
	log.clear();
	log << "Draw calls: " << get_draw_calls();
	render_shadow_text(log.str(), 10.0f, 70.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	log.str("");
	log.clear();
	log << "Position: " << camera.get_position().x << ", " << camera.get_position().y << ", " << camera.get_position().z;
	render_shadow_text(log.str(), 10.0f, 95.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	log.str("");
	log.clear();
	log << "Velocity: " << player->get_velocity().x << ", " << player->get_velocity().y << ", " << player->get_velocity().z;
	render_shadow_text(log.str(), 10.0f, 120.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	log.str("");
	log.clear();
	log << "Forward: " << camera.get_forward().x << ", " << camera.get_forward().y << ", " << camera.get_forward().z;
	render_shadow_text(log.str(), 10.0f, 145.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	log.str("");
	log.clear();
	log << "Right: " << camera.get_right().x << ", " << camera.get_right().y << ", " << camera.get_right().z;
	render_shadow_text(log.str(), 10.0f, 170.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	log.str("");
	log.clear();
	log << "Up: " << camera.get_up().x << ", " << camera.get_up().y << ", " << camera.get_up().z;
	render_shadow_text(log.str(), 10.0f, 195.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	


	set_stroke_color(1.0f, 1.0f, 1.0f, 1.0f);
	set_stroke_width(3.0f);
	nvgBeginPath(m_nvg_ctx);
	auto center = Application::get_center();
	nvgMoveTo(m_nvg_ctx, center.x - 10.0f, center.y);
	nvgLineTo(m_nvg_ctx, center.x + 10.0f, center.y);
	nvgMoveTo(m_nvg_ctx, center.x, center.y - 10.0f);
	nvgLineTo(m_nvg_ctx, center.x, center.y + 10.0f);
	nvgStroke(m_nvg_ctx);


    end_frame();
}

void HUDRenderer::begin_frame() {
	nvgBeginFrame(m_nvg_ctx, static_cast<float>(Application::get_width()), static_cast<float>(Application::get_height()), 1.0f);
}
void HUDRenderer::end_frame() {
	nvgEndFrame(m_nvg_ctx);
}

void HUDRenderer::render_text(const std::string& text, float x, float y){
    nvgText(m_nvg_ctx, x, y, text.c_str(), nullptr);
}

void HUDRenderer::render_shadow_text(const std::string& text, float x, float y, float r, float g, float b, float a) {
	set_fill_color(0.0f, 0.0f, 0.0f, 1.0f);
	render_text(text, x + 2.0f, y + 2.0f);
	set_fill_color(r, g, b, a);
	render_text(text, x, y);
}


void HUDRenderer::set_font(const std::string& font_name){
    nvgFontFace(m_nvg_ctx, font_name.c_str());
}

void HUDRenderer::set_font_size(float font_size){
    nvgFontSize(m_nvg_ctx, font_size);
}

void HUDRenderer::set_text_align(int align){
    nvgTextAlign(m_nvg_ctx, align);
}

void HUDRenderer::set_fill_color(float r, float g, float b, float a){
    nvgFillColor(m_nvg_ctx, nvgRGBA(
        static_cast<unsigned char>(255 * r),
        static_cast<unsigned char>(255 * g),
        static_cast<unsigned char>(255 * b),
        static_cast<unsigned char>(255 * a)
    ));
}

void HUDRenderer::set_stroke_color(float r, float g, float b, float a) {
	nvgStrokeColor(m_nvg_ctx, nvgRGBA(
		static_cast<unsigned char>(255 * r),
		static_cast<unsigned char>(255 * g),
		static_cast<unsigned char>(255 * b),
		static_cast<unsigned char>(255 * a)
	));
}

void HUDRenderer::set_stroke_width(float width) {
	nvgStrokeWidth(m_nvg_ctx, width);
}


