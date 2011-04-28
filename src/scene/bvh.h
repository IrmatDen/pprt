#ifndef PPRT_BVH_H
#define PPRT_BVH_H

#include "aabb.h"
#include "geometry.h"
#include "ray.h"

#include "../memory.h"

#include <limits>
#include <set>
#include <vector>

// Object typename must be a pointer & provide the following methods:
// AABB getAABB()
// bool hit(const Ray &, IntersectionInfo&)
// Point3 position()
// 
// ObjectContainer is expected to provide the following types:
// const_iterator
// ...and methods:
// default ctor
// Object* at()
// size_t size()
// const_iterator begin()
// const_iterator end()
// void push_back(Object)
template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf = 4>
class BVH
{
public:
	BVH();
	~BVH();

	void	build(const ObjectContainer &objects);
	void	clear();

	const Object*	findClosest(const Ray &ray, IntersectionInfo &ii) const;

private:
	struct _MM_ALIGN16 BVHNode
	{
		BVHNode() : isLeaf(false), left(0), right(0)	{}
		~BVHNode()
		{
			if (!isLeaf)	// objects are deleted by the scene
			{
				memory::destroy(left);
				memory::destroy(right);
			}
			else
			{
				memory::destroy(objects);
			}
		}

		AABB			aabb;
		bool			isLeaf;

		union
		{
			struct
			{
				BVHNode	*	left;
				BVHNode	*	right;
			};
			struct
			{
				int			objCount;
				Object **	objects;
			};
		};
	};

	enum SplitAxis
	{
		SA_None = -1,
		SA_X,
		SA_Y,
		SA_Z
	};

private:
	void		setAABBFor(AABB &aabb, const ObjectContainer &objects) const;

	void		buildSubTree(BVHNode &currentNode, const ObjectContainer &objects);

	void		splitObjects(SplitAxis sa, const AABB &aabb, const ObjectContainer &objects,
							 ObjectContainer &leftObjects, ObjectContainer &rightObjects,
							 AABB &leftAABB, AABB &rightAABB
							) const;

	// Search for best axis to cut a given aabb
	SplitAxis	bestNodeCut(const AABB &aabb, const ObjectContainer &objects) const;

	const Object*	innerTraverse(BVHNode *node, const Ray &ray, float &tmin, float &tmax, IntersectionInfo &ii) const;

private:
	BVHNode *root;
};

#include "bvh.inl"

#endif
