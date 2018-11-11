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
	Renderer() : m_wireframe(false) {
		m_shader = std::make_unique<ShaderProgram>();
	}
	virtual void render(const std::vector<std::shared_ptr<M>>& models, Camera& camera, const std::shared_ptr<Light>& light) = 0;

	bool intersects_frustum(const std::shared_ptr<M>& model, const Camera& camera) {
		auto p = camera.get_frustum_planes();

		auto center = model->get_center();
		auto radius = model->get_bounding_radius();

		for (int i : {FRUSTUM_BOTTOM, FRUSTUM_LEFT, FRUSTUM_TOP, FRUSTUM_BOTTOM, FRUSTUM_FAR}){
			if (glm::dot(p[i].n, center) + p[i].d + radius <= 0.0f) {
				return false;
			}
		}

		return true;
	}

	void set_clip_plane(const glm::vec4& clip_plane) {
		m_clip_plane = clip_plane;
	}

	void set_shadow_map(const std::shared_ptr<ITexture>& shadow_map) {
		m_shadow_map = shadow_map;
	}

	void toggle_wireframe() {
		m_wireframe = !m_wireframe;
	}

protected:
	glm::vec4 m_clip_plane;
	std::shared_ptr<ITexture> m_shadow_map;
	std::unique_ptr<ShaderProgram> m_shader;

	bool m_wireframe;
};