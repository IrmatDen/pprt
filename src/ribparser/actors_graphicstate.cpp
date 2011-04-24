#include "actors_graphicstate.h"
#include "actors_options.h"
#include "actors_transform.h"

#include "../memory.h"
#include "../scene/scene.h"
#include "../scene/color.h"

using namespace std;

//---------------------------------------------
// worldBegin_a

worldBegin_a::worldBegin_a(Scene &scn)
	: scene(scn)
{
}

void worldBegin_a::operator()(const iterator_t&, const iterator_t&) const
{
	// Finalize camera
	const float aspect = static_cast<float>(CameraSettings::resX * CameraSettings::pix_aspectRatio / CameraSettings::resY);
		
	float screenExtents[4];
	if (aspect > 1.f)
	{
		screenExtents[0] = -aspect;
		screenExtents[1] =  aspect;
		screenExtents[2] = -1.f;
		screenExtents[3] =  1.f;
	}
	else
	{
		screenExtents[0] = -1.f;
		screenExtents[1] =  1.f;
		screenExtents[2] = -1.f / aspect;
		screenExtents[3] =  1.f / aspect;
	}

	Camera::CameraModel cm = Camera::CM_Orthographic;
	if (CameraSettings::projType == "\"perspective\"")
		cm = Camera::CM_Perspective;

	scene.camera().finalize(cm, TransformStack::currentTransform, aspect, deg2rad(CameraSettings::fov),
							CameraSettings::resX, CameraSettings::resY, (float)CameraSettings::hither, (float)CameraSettings::yon,
							screenExtents);

	// Reset current transform
	TransformStack::currentTransform = Matrix4::identity();
}

//---------------------------------------------
// shaderParams_a

string			shaderParams_a::paramName;
ShaderParams	shaderParams_a::params;

void shaderParams_a::operator()(const Color &col) const
{
	params.push_back(ShaderParam(paramName, PT_Color, col));
	paramName.swap(string());
}
	
void shaderParams_a::operator()(double d) const
{
	params.push_back(ShaderParam(paramName, PT_Float, (float)d));
	paramName.swap(string());
}

//---------------------------------------------
// GraphicState

GraphicState::GraphicState()
	: _color(1,1,1), _opacity(1,1,1)
{
}

//---------------------------------------------
// GraphicStateStack

GraphicStateStack::Stack	GraphicStateStack::stateStack;
GraphicState				GraphicStateStack::current;

//---------------------------------------------
// activateCurrentShaderParams_a

void activateCurrentShaderParams_a::operator()(const iterator_t&, const iterator_t&) const
{
	GraphicStateStack::current.shaderParams = shaderParams_a::params;
	shaderParams_a::params.swap(ShaderParams());
}

//---------------------------------------------
// attributeBegin_a

void attributeBegin_a::operator()(const iterator_t &s, const iterator_t &e) const
{
	transformBegin_a()(s, e);

	GraphicStateStack::stateStack.push(GraphicStateStack::current);
}

//---------------------------------------------
// attributeEnd_a

void attributeEnd_a::operator()(const iterator_t &s, const iterator_t &e) const
{
	transformEnd_a()(s, e);

	GraphicStateStack::current = GraphicStateStack::stateStack.top();
	GraphicStateStack::stateStack.pop();
}

//---------------------------------------------
// newPointLight_a

Point3	newPointLight_a::pos;
Color	newPointLight_a::color;

newPointLight_a::newPointLight_a(Scene &scn)
	: scene(scn)
{
}
	
void newPointLight_a::operator()(const iterator_t&, const iterator_t&) const
{
	Light *l(memory::construct<Light>());
	l->pos		= pos;
	l->color	= color;
	scene.addLight(l);

	// Reset fields to allow for defaults
	pos = Point3(0);
	color = Color(0);
}
