#pragma once

#include <map>
#include <any>

class ConfigFile {
public:
	using ValueType = std::string;

	ConfigFile(const std::string& file);

	template<typename T>
	T get(const std::string& key) const {
		auto it = m_values.find(key);
		if (m_values.find(key) == m_values.end()) {
			LOG("Key: " << key << " does not exist in the configuration file" << std::endl);
			return T{};
		}
		return convert_value<T>(it->second);
	}
private:
	template<typename T>
	auto convert_value(const ValueType& value) const -> std::enable_if_t<std::is_same<T, std::string>::value, T> {
		return value;
	}

	template<typename T>
	auto convert_value(const ValueType& value) const -> std::enable_if_t<std::is_same<T, float>::value, T> {
		return std::stof(value);
	}

	template<typename T>
	auto convert_value(const ValueType& value) const -> std::enable_if_t<std::is_same<T, int>::value, T> {
		return std::stoi(value);
	}


	void parse(const std::string& file);

	std::map<std::string, ValueType> m_values;
};