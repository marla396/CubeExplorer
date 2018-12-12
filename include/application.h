#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <unordered_map>

#include "gl.h"

#include <glm/glm.hpp>

class Game;
struct NVGcontext;

class Application{
public:
	Application();
	~Application();

    bool initialize(int argc, char** argv);
	void run();
    
	static void set_resolution(size_t width, size_t height);
	static size_t get_width();
	static size_t get_height();
	static glm::vec2 get_center();
	static float get_render_time();
	static float get_fps();
	static std::string get_root_directory();
	static bool key_down(int key);
	static void exit();
	static void update_viewport();
	static void register_resize_callback(const std::function<void(size_t, size_t)>& callback);
	static void toggle_show_cursor();
	static void add_time_warp_factor(float inc);
	static float get_time_warp();
private:
	const size_t DEFAULT_WIDTH = 800;
	const size_t DEFAULT_HEIGHT = 600;
	const float DEFAULT_MAX_FPS = 60.0f;


	bool parse_arguments(const std::vector<std::string>& arguments);
	void print_info() const;

	static void glfw_error_callback(int error, const char* description);
	static void on_resize(GLFWwindow* window, int width, int height);
	static void on_cursor(GLFWwindow* window, double xpos, double ypos);
	static void on_key(GLFWwindow* window, int key, int scan_code, int action, int mods);

	void start_render_timer();
	void stop_render_timer();
	void sleep_until_next_frame();


	GLFWwindow* m_window;
	NVGcontext* m_nvg_ctx;
	std::string m_root_directory;

	std::unique_ptr<Game> m_game;
	size_t m_width;
	size_t m_height;

	double m_center_x;
	double m_center_y;
	bool m_fullscreen;
	bool m_benchmark_water;

	float m_time;
	float m_previous_time;
	float m_max_fps;
	float m_time_warp_factor;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_render_start;
	float m_render_time;

	std::vector<std::function<void(size_t, size_t)>> m_resize_callbacks;
};

