#ifndef CRT_BVH_H
#define CRT_BVH_H

#include "aabb.h"
#include "ray.h"
#include "scene.h"

#include "../common.h"

class BVH
{
public:
	BVH() : root(0) {}
	~BVH();

	void			build(const Scene::Geometries &objects);

	const Geometry*	findClosest(const Ray &ray, Real &t) const;
	size_t			gatherAlong(const Ray &ray, Real &t, Geometry **accum, Real *distances, size_t maxObj) const;

private:
	struct _MM_ALIGN16 BVHNode
	{
		static const int MaxObjPerLeaf = 6;

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
				Geometry **	objects;
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
	void		setAABBFor(AABB &aabb, const Scene::Geometries &objects) const;

	void		buildSubTree(BVHNode &currentNode, const Scene::Geometries &objects);

	void		splitObjects(SplitAxis sa, const AABB &aabb, const Scene::Geometries &objects,
							 Scene::Geometries &leftObjects, Scene::Geometries &rightObjects,
							 AABB &leftAABB, AABB &rightAABB
							) const;

	// Search for best axis to cut a given aabb
	SplitAxis	bestNodeCut(const AABB &aabb, const Scene::Geometries &objects) const;

	const Geometry*	innerTraverse(BVHNode *node, const Ray &ray, Real &t) const;
	void			innerGather(BVHNode *node, const Ray &ray, Real &t, Geometry **accum, Real *distances, size_t &startIdx, size_t maxObj) const;

private:
	BVHNode *root;
};

#endif
