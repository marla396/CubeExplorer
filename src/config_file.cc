#include <string>
#include <fstream>
#include <sstream>

#include "config_file.h"
#include "gl.h"

ConfigFile::ConfigFile(const std::string& file) {
	parse(file);
}

void ConfigFile::parse(const std::string& file) {

	std::ifstream config(file);

	if (!config) {
		LOG("Failed to read config: " << file << std::endl);
		return;
	}

	std::string line;
	int line_number = 0;

	while (std::getline(config, line)) {
		line_number++;
		std::istringstream iss(line);
		std::string key;
		if (std::getline(iss, key, '=')) {
			std::string v;
			if (std::getline(iss, v)) {
				m_values[key] = v;
			}
		}
		else {
			LOG("Failed to parse config: " << file << "\n" << "No \'=\' found at line " << line_number << std::endl);
		}
	}
}