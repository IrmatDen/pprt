#include "compiled_shader.h"

#include "../sse.h"

using namespace boost;

void CompiledShader::faceForward()
{
	// 2 parameters expected: normal & incident
	const Vector3 &n = execStack.pop<Vector3>();
	const Vector3 &i = execStack.pop<Vector3>();

	Vector3 &ng = get<Vector3>(rtVarTable[Ng]);

	float din = (dot(-i, ng) < 0 ? -1.f : 1.f);

	execStack.push(n * din);
}

void CompiledShader::normalize()
{
	Vector3 &v = execStack.pop<Vector3>();
	execStack.push(Vectormath::Aos::normalize(v));
}

void CompiledShader::reflect()
{
	// 2 parameters expected: incident & normal
	const Vector3 &i = execStack.pop<Vector3>();
	const Vector3 &n = execStack.pop<Vector3>();

	execStack.push(i - n * 2 * dot(i, n));
}