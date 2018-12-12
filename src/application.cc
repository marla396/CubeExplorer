#include "application.h"
#include "game.h"

#include <nanovg/nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg/nanovg_gl.h>

#include <thread>
#include <iomanip>

//Not very nice, but is required
Application* g_app;

Application::Application(){
	g_app = this;
}

Application::~Application(){

}

bool Application::initialize(int argc, char** argv){

	if (!parse_arguments(std::vector<std::string>(argv, argv + argc))) {
		return false;
	}

	glfwSetErrorCallback(Application::glfw_error_callback);
	if (!glfwInit())
		return false;

#ifndef _WIN32
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	if (m_fullscreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		m_width = static_cast<size_t>(mode->width);
		m_height = static_cast<size_t>(mode->height);

		m_window = glfwCreateWindow(mode->width, mode->height, "Cube Explorer", monitor, NULL);
	}
	else {
		m_window = glfwCreateWindow(static_cast<int>(m_width), static_cast<int>(m_height), "Cube Explorer", NULL, NULL);
	}
	if (!m_window)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

#ifdef _WIN32
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return false;
	}
#endif

	glfwSetWindowSizeCallback(m_window, Application::on_resize);
	glfwSetCursorPosCallback(m_window, Application::on_cursor);
	glfwSetKeyCallback(m_window, Application::on_key);
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, 1);
	
	Application::toggle_show_cursor();

	print_info();

	m_nvg_ctx = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

	if (m_nvg_ctx == nullptr)
		return false;

	m_game = std::make_unique<Game>(m_nvg_ctx);
	m_time = 0.0f;
	m_time_warp_factor = 1.0f;

	set_resolution(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	return true;
}

void Application::run() {

	if (m_benchmark_water) {
		m_game->benchmark_water();
	}
	else {

		GLC(glClearColor(0.36f, 0.74f, 1.0f, 1.0f));

		while (!glfwWindowShouldClose(m_window))
		{
			start_render_timer();
			sleep_until_next_frame();

			m_game->on_update(m_time, m_time - m_previous_time);
			m_game->on_render();

			glfwSwapBuffers(m_window);
			glfwPollEvents();

			stop_render_timer();
			m_previous_time = m_time;
			m_time += (m_render_time / 1000000.0f) * m_time_warp_factor;
		}
	}
	
	nvgDeleteGL3(m_nvg_ctx);
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Application::set_resolution(size_t width, size_t height){
	g_app->m_width = width;
	g_app->m_height = height;
	g_app->m_center_x = static_cast<double>(width) / 2.0;
	g_app->m_center_y = static_cast<double>(height) / 2.0;

	g_app->update_viewport();
}

size_t Application::get_width(){
	return g_app->m_width;
}

size_t Application::get_height(){
	return g_app->m_height;
}

glm::vec2 Application::get_center() {
	return { static_cast<float>(g_app->m_center_x), static_cast<float>(g_app->m_center_y) };
}

float Application::get_render_time(){
	return g_app->m_render_time / 1000.0f;
}

float Application::get_fps(){
	return 1.0f / (g_app->m_render_time / 1000000.0f); 
}

std::string Application::get_root_directory() {
	return g_app->m_root_directory;
}

bool Application::key_down(int key) {
	return glfwGetKey(g_app->m_window, key) == GLFW_PRESS;
}

void Application::exit() {
	glfwSetWindowShouldClose(g_app->m_window, GLFW_TRUE);
}


void Application::update_viewport() {
	GLC(glViewport(0, 0, g_app->m_width, g_app->m_height));
}

void Application::register_resize_callback(const std::function<void(size_t, size_t)>& callback){
	g_app->m_resize_callbacks.push_back(callback);
}

void Application::toggle_show_cursor() {
	static bool showing_cursor = true;
	showing_cursor = !showing_cursor;

	if (showing_cursor)
		glfwSetInputMode(g_app->m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(g_app->m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

}

void Application::add_time_warp_factor(float inc) {
	g_app->m_time_warp_factor += inc;
}

float Application::get_time_warp(){
	return g_app->m_time_warp_factor;
}

bool Application::parse_arguments(const std::vector<std::string>& arguments) {

	m_root_directory = ".";
	m_max_fps = DEFAULT_MAX_FPS;
	m_width = DEFAULT_WIDTH;
	m_height = DEFAULT_HEIGHT;
	m_fullscreen = false;
	m_benchmark_water = false;

	auto check_end = [&arguments](auto& it) { return ++it != arguments.end(); };

	for (auto it = arguments.begin(); it != arguments.end(); it++) {
		
		if (*it == "--rootdir" || *it == "-r") {
			if (check_end(it)) {
				m_root_directory = *it;
#ifdef _WIN32
				char delim = '\\';
#else
				char delim = '/';
#endif
				if (m_root_directory.back() == delim) {
					m_root_directory.pop_back();
				}
			}
			else {
				std::cerr << "Root directory option requires an argument";
			}
		}
		else if (*it == "--maxfps" || *it == "-m") {
			if (check_end(it)) {
				try {
					m_max_fps = std::stof(*it);
				}
				catch (std::exception&) {
					std::cerr << "Invalid max FPS value." << std::endl;
				}
			}
			else {
				std::cerr << "Max FPS option requires an argument";
			}
		}
		else if (*it == "--fullscreen" || *it == "-f") {
			m_fullscreen = true;
		}
		else if (*it == "--benchmark" || *it == "-b") {
			m_benchmark_water = true;
		}
	}

	return true;
}

void Application::print_info() const{

	int max_tex, max_patch_verts;
	int max_compute_groups[3];
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_tex);
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &max_patch_verts);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_compute_groups[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_compute_groups[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_compute_groups[2]);

	std::cout	<< "Vendor: " << glGetString(GL_VENDOR) << "\n"
				<< "Renderer: " << glGetString(GL_RENDERER) << "\n"
				<< "Version: " << glGetString(GL_VERSION) << "\n"
				<< "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
				<< "Max tex units: " << max_tex << "\n"
				<< "Max patch verts: " << max_patch_verts << "\n"
				<< "Max compute work group: (" << max_compute_groups[0] << ", " << max_compute_groups[1] << ", " << max_compute_groups[2] << ")\n"
				<< std::endl << std::endl;
}

void Application::glfw_error_callback(int error, const char* description){
	MAYBE_UNUSED(error);
	MAYBE_UNUSED(description);
	
	LOG("GLFW error [" << error << "] " << description << std::endl);
}


void Application::on_resize(GLFWwindow* window, int width, int height) {
	UNUSED(window);



	g_app->set_resolution(static_cast<size_t>(width), static_cast<size_t>(height));
	g_app->m_game->on_resize(g_app->m_width, g_app->m_height);

	for (const auto& callback : g_app->m_resize_callbacks){
		callback(g_app->m_width, g_app->m_height);
	}
}

void Application::on_cursor(GLFWwindow* window, double xpos, double ypos) {
	UNUSED(window);

	float x = static_cast<float>(xpos);
	float y = static_cast<float>(ypos);

	float dx = x - static_cast<float>(g_app->m_center_x);
	float dy = y - static_cast<float>(g_app->m_center_y);

	g_app->m_game->on_cursor(dx, dy);

	glfwSetCursorPos(g_app->m_window, g_app->m_center_x, g_app->m_center_y);
}

void Application::on_key(GLFWwindow* window, int key, int scan_code, int action, int mods) {
	UNUSED(window);

	g_app->m_game->on_key(key, scan_code, action, mods);
}



void Application::start_render_timer(){
	m_render_start = std::chrono::high_resolution_clock::now();
}

void Application::stop_render_timer(){
	auto render_time = std::chrono::high_resolution_clock::now() - m_render_start;
	m_render_time = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(render_time).count());
}

void Application::sleep_until_next_frame(){
	float idle = (1.0f / (m_max_fps / 1000000.0f)) - m_render_time;

	if (idle > 0){
		m_time += idle / 1000000.0f;
		std::this_thread::sleep_for(std::chrono::duration<float, std::micro>(idle));
	}
}