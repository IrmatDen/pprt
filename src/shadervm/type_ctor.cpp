#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::colorCtor()
{
	Color col;
	--esp;
	col.r = (float)boost::get<Real>(esp->second);
	--esp;
	col.g = (float)boost::get<Real>(esp->second);
	--esp;
	col.b = (float)boost::get<Real>(esp->second);

	esp->first = VT_Color;
	esp->second = col;
	++esp;
}
