#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::color4Ctor()
{
	assert(execStack.size() >= 4);

	Color4 col;
	col.r = (float)any_cast<double>(execStack.top().second); execStack.pop();
	col.g = (float)any_cast<double>(execStack.top().second); execStack.pop();
	col.b = (float)any_cast<double>(execStack.top().second); execStack.pop();
	col.a = (float)any_cast<double>(execStack.top().second); execStack.pop();

	execStack.push(make_pair(VT_Color, col));
}
