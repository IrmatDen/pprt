#include "vector3.h"

template<> float Vector3<float>::length() const
{
	return sqrtf(this->dot(*this));
}

template<> double Vector3<double>::length() const
{
	return sqrt(this->dot(*this));
}
