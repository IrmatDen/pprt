#include "compiled_shader.h"

#include "../sse.h"

using namespace boost;

void CompiledShader::faceForward()
{
	// 2 parameters expected: normal & incident
	const Vector3 &n = execStack.pop<Vector3>();
	const Vector3 &i = execStack.pop<Vector3>();

	const Vector3 &ng = get<Vector3>(rtVarTable[Ng]);

	execStack.push(dot(-i, ng) < 0 ? -n : n);
}

void CompiledShader::normalize()
{
	const Vector3 &v = execStack.pop<Vector3>();
	execStack.push(Vectormath::Aos::normalize(v));
}

void CompiledShader::reflect()
{
	// 2 parameters expected: incident & normal
	const Vector3 &i = execStack.pop<Vector3>();
	const Vector3 &n = execStack.pop<Vector3>();

	execStack.push(i - n * 2 * dot(i, n));
}