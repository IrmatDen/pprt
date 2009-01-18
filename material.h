#ifndef CRT_MATERIAL_H
#define CRT_MATERIAL_H

#include "vector3.h"
#include "color4.h"

class Light;
class Scene;

class Material
{
public:
	Color4		shade(const Vec3f &p, const Vec3f &n, const Light &l) const;

public:
	Color4		ambient;
	Color4		diffuse;
	float		reflexivity;
};

#endif