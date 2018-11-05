#pragma once
#include <iostream>


#ifndef _WIN32
#include <GL/gl.h>
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glext.h>
#include <GL/glx.h>
#include <GLFW/glfw3.h>
#endif

#ifdef _WIN32
//#include <glew/include/glew.h>
#include <glad/include/glad.h>
#include <gl/gl.h>
#include <glfw/include/glfw3.h>

#endif

constexpr float PI = 3.14159265359f;

void clear_OpenGL_errors();
bool check_OpenGL_errors(const char* function, const char* file, int line);
size_t get_draw_calls();

void increment_draw_calls();
void check_framebuffer_status();

#define NOP ((void)0)

#ifdef _WIN32
#define TRAP				__debugbreak()
#else
#define TRAP				NOP//asm("int 0x3")
#endif

#define GL_ASSERT(stmt)	if (!(stmt)) TRAP;

#define GLC(stmt)			clear_OpenGL_errors(); \
							stmt; \
							GL_ASSERT(check_OpenGL_errors(#stmt, __FILE__, __LINE__));

#define CHECK_FRAME_BUFFER() check_framebuffer_status()

#define DRAW_CALL(stmt) increment_draw_calls(); \
						stmt;

#define LOG(msg) std::cout << msg << std::endl;

#define UNUSED(x) (void)x;