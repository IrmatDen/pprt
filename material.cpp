#include <boost/algorithm/string.hpp>

#include "material.h"
#include "light.h"

bool operator==(MaterialPtr m1, const std::string &name)
{
	return boost::iequals(m1->name, name);
}

Color4 Material::shade(const Vec3f &p, const Vec3f &n, const Light &l) const
{
	Vec3f L2P = l.pos - p;

	Color4 res;
	if (n.dot(L2P) <= 0)
		return res;

	float t = L2P.length();
	if (t < 0)
		return res;

	float lambert = L2P.normalize().dot(n);
	res += l.color * color * lambert;
	res.a = 1;

	return res.clamp();
}
