#ifndef PPRT_ACTORS_GRAPHICSTATE_H
#define PPRT_ACTORS_GRAPHICSTATE_H

#include "parser_typedefs.h"

#include "../shadervm/shader_param.h"
#include "../scene/color.h"

#include <stack>
#include <string>

class Scene;

struct shaderParams_a
{
	void operator()(const Color &col) const;
	void operator()(double d) const;

	static std::string	paramName;
	static ShaderParams	params;
};

struct worldBegin_a
{
	worldBegin_a(Scene &scn);
	void operator()(const iterator_t&, const iterator_t&) const;
	
	Scene	&scene;
};

struct GraphicState
{
	GraphicState();

	Color	_color;
	Color	_opacity;
	
	ShaderParams	shaderParams;
	std::string		surfaceShader;
};

struct GraphicStateStack
{
	typedef std::stack<GraphicState, std::deque<GraphicState, memory::AllocAlign16<GraphicState>>> Stack;

	static Stack		stateStack;
	static GraphicState	current;
};

struct activateCurrentShaderParams_a
{
	void operator()(const iterator_t&, const iterator_t&) const;
};

struct attributeBegin_a
{
	void operator()(const iterator_t &s, const iterator_t &e) const;
};

struct attributeEnd_a
{
	void operator()(const iterator_t &s, const iterator_t &e) const;
};

struct newPointLight_a
{
	newPointLight_a(Scene &scn);
	void operator()(const iterator_t&, const iterator_t&) const;

	Scene &scene;

	static Point3	pos;
	static Color	color;
};

#endif
