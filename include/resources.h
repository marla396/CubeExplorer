#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

class Resources {
public:
	static std::string resolve_path(const std::string& relative_path);
	static std::string resolve_shader_path(const std::string& relative_shader_path);
	static std::string resolve_texture_path(const std::string& relative_texture_path);
	static std::string resolve_root_path(const std::string& relative_root_path);
	static std::string read_file(const std::string& path);
};