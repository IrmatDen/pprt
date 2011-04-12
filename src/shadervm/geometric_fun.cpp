#include "compiled_shader.h"

#include "../sse.h"

using namespace boost;

void CompiledShader::faceForward()
{
	// 2 parameters expected: normal & incident
	--esp;
	const Vector3 &n = get<Vector3>(esp->second);
	--esp;
	const Vector3 &i = get<Vector3>(esp->second);

	Vector3 &ng = get<Vector3>(varTable[Ng].content);

	float din = (dot(-i, ng) < 0 ? -1.f : 1.f);

	Vector3 fw(n * din);
	
	esp->first = VT_Vector;
	esp->second = fw;
	++esp;
}

void CompiledShader::normalize()
{
	--esp;
	Vector3 &v = get<Vector3>(esp->second);
	v = Vectormath::Aos::normalize(v);
	
	//esp->first = VT_Vector;
	//esp->second = v.normalized();
	++esp;
}

void CompiledShader::reflect()
{
	// 2 parameters expected: incident & normal
	--esp;
	const Vector3 &i = get<Vector3>(esp->second);
	--esp;
	const Vector3 &n = get<Vector3>(esp->second);

	Vector3 r(i - n * 2 * dot(i, n));
	
	esp->first = VT_Vector;
	esp->second = r;
	++esp;
}