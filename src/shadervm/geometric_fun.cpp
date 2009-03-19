#include "compiled_shader.h"

#include "../scene/vector3.h"

using namespace boost;

void CompiledShader::faceForward()
{
	// 2 parameters expected: normal & incident
	--esp;
	const Vec3 &n = esp->second;
	--esp;
	const Vec3 &i = esp->second;

	Vec3 &ng = varTable[Ng].content;

	float din = ((-i).dot(ng) < 0 ? -1.f : 1.f);

	Vec3 fw(n * din);
	
	esp->first = VT_Vector;
	esp->second = fw;
	++esp;
}

void CompiledShader::normalize()
{
	--esp;
	Vec3 &v = esp->second;
	v.normalize();
	
	//esp->first = VT_Vector;
	//esp->second = v.normalized();
	++esp;
}

void CompiledShader::reflect()
{
	// 2 parameters expected: incident & normal
	--esp;
	const Vec3 &i = esp->second;
	--esp;
	const Vec3 &n = esp->second;

	Vec3 r(i.reflect(n));
	
	esp->first = VT_Vector;
	esp->second = r;
	++esp;
}