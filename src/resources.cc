#include "resources.h"
#include "application.h"

std::string Resources::resolve_path(const std::string& relative_path) {
	std::ostringstream oss;
	std::string root = Application::get_root_directory();

	oss << root;
	if (relative_path[0] != '/')
		oss << "/";
	oss << relative_path;

	std::string out = oss.str();

#ifdef _WIN32
	std::replace(out.begin(), out.end(), '/', '\\');
#endif

	return out;
}

std::string Resources::resolve_shader_path(const std::string& relative_shader_path) {
	return resolve_path("/res/shaders/" + relative_shader_path);
}

std::string Resources::resolve_texture_path(const std::string& relative_texture_path){
	return resolve_path("/res/textures/" + relative_texture_path);
}

std::string Resources::read_file(const std::string& path) {
	std::ifstream file(path, std::ios::ate);

	if (!file) {
		LOG("Failed to open " << path << std::endl);
		return "";
	}

	std::string ret;

	ret.reserve(file.tellg());
	file.seekg(0, std::ios::beg);

	ret.assign((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	
	return ret;
}