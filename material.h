#ifndef CRT_MATERIAL_H
#define CRT_MATERIAL_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "vector3.h"
#include "color4.h"

class Light;
class Scene;

class Material
{
public:
	Color4		shade(const Vec3 &p, const Vec3 &n, const Light &l) const;

public:
	std::string		name;
	Color4			color;
	float			reflexivity;
};

typedef boost::shared_ptr<Material> MaterialPtr;

bool operator==(MaterialPtr m1, const std::string &name);

#endif