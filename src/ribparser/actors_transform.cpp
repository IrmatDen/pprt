#include "actors_transform.h"

TransformStack::XformStack	TransformStack::stack;
Matrix4						TransformStack::currentTransform(Matrix4::identity());

void identity_a::operator()(const iterator_t&, const iterator_t&) const
{
	TransformStack::currentTransform = Matrix4::identity();
}

void translate_a::operator()(const NonAlignedVec3 &vec) const
{
	TransformStack::currentTransform = TransformStack::currentTransform * Matrix4::translation(vec);
}

double	rotate_a::angleDegrees = 0.f;
Vector3	rotate_a::axis;
void rotate_a::operator()(const iterator_t&, const iterator_t&) const
{
	const Matrix4 rot = Matrix4::rotation(deg2rad(angleDegrees), normalize(axis));
	TransformStack::currentTransform = TransformStack::currentTransform * rot;
}

void transformBegin_a::operator()(const iterator_t&, const iterator_t&) const
{
	TransformStack::stack.push(TransformStack::currentTransform);
}

void transformEnd_a::operator()(const iterator_t&, const iterator_t&) const
{
	TransformStack::currentTransform = TransformStack::stack.top();
	TransformStack::stack.pop();
}
