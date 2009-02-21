#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::color4Ctor()
{
	Color4 col;
	--esp;
	col.r = (float)boost::get<Real>(esp->second);
	--esp;
	col.g = (float)boost::get<Real>(esp->second);
	--esp;
	col.b = (float)boost::get<Real>(esp->second);
	--esp;
	col.a = (float)boost::get<Real>(esp->second);

	esp->first = VT_Color;
	esp->second = col;
	++esp;
}
