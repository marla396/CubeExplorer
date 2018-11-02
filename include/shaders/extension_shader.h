#pragma once

#include "shader.h"

class ExtensionShader {
public:
	ExtensionShader(Shader* parent);
	virtual ~ExtensionShader();
protected:
	virtual void get_uniform_locations() = 0;
	Shader* m_parent;
};