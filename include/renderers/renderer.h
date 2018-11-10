#pragma once

#include "gl.h"
#include "world/light.h"
#include "shaders/shader.h"
#include "models/model.h"
#include "../camera.h"

#include <glm/glm.hpp>

template<typename ShaderProgram, typename M>
class Renderer{
public:
	Renderer() {
		m_shader = std::make_unique<ShaderProgram>();
	}
	virtual void render(const std::vector<std::shared_ptr<M>>& models, Camera& camera, const std::shared_ptr<Light>& light) = 0;
	
	void set_clip_plane(const glm::vec4& clip_plane) {
		m_clip_plane = clip_plane;
	}

	void set_shadow_map(const std::shared_ptr<ITexture>& shadow_map) {
		m_shadow_map = shadow_map;
	}

protected:
	glm::vec4 m_clip_plane;
	std::shared_ptr<ITexture> m_shadow_map;
	std::unique_ptr<ShaderProgram> m_shader;
};