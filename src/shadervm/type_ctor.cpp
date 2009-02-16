#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::color4Ctor()
{
	Color4 col;
	--esp;
	col.r = (float)any_cast<double>(esp->second);
	--esp;
	col.g = (float)any_cast<double>(esp->second);
	--esp;
	col.b = (float)any_cast<double>(esp->second);
	--esp;
	col.a = (float)any_cast<double>(esp->second);

	*esp = make_pair(VT_Color, col);
	++esp;
}
