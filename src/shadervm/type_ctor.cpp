#include <cassert>

#include "compiled_shader.h"

using namespace std;
using namespace boost;

void CompiledShader::color4Ctor()
{
	//! \todo replace assert by exception
	assert(execStack.size() >= 4);

	//! \todo handle bad_any_cast.
	Color4 col;
	col.a = (float)any_cast<double>(execStack.top().second); execStack.pop();
	col.b = (float)any_cast<double>(execStack.top().second); execStack.pop();
	col.g = (float)any_cast<double>(execStack.top().second); execStack.pop();
	col.r = (float)any_cast<double>(execStack.top().second); execStack.pop();

	execStack.push(make_pair(TF_Color, col));
}
