#include "compiled_shader.h"

#include "../scene/vector3.h"

using namespace boost;

void CompiledShader::reflect()
{
	// 2 parameters expected: incident & normal
	--esp;
	const Vec3 &i = *any_cast<Vec3>(&(esp->second));
	--esp;
	const Vec3 &n = *any_cast<Vec3>(&(esp->second));

	Vec3 r = i.reflect(n);
	
	*esp = std::make_pair(VT_Vector, r);
	++esp;
}