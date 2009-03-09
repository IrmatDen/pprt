#ifndef CRT_BVH_H
#define CRT_BVH_H

#include "aabb.h"
#include "ray.h"
#include "scene.h"

class BVH
{
public:
	BVH() : root(0) {}
	~BVH();

	void			build(const Scene::Geometries &objects);

	const Geometry*	findClosest(const Ray &ray, Real &t) const;
	size_t			gatherAlong(const Ray &ray, Real &t, Geometry **accum, Real *distances, size_t maxObj) const;

private:
	struct BVHNode
	{
		BVHNode() : isLeaf(false), left(0), right(0)	{}
		~BVHNode()
		{
			if (!isLeaf)	// objects are deleted by the scene
			{
				delete left;
				delete right;
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
				Geometry *	geoLeft;
				Geometry *	geoRight;
			};
		};
	};

	enum SplitAxis
	{
		SA_X,
		SA_Y,
		SA_Z
	};

private:
	void		setAABBFor(AABB &aabb, const Scene::Geometries &objects) const;

	void		buildSubTree(BVHNode &currentNode, const Scene::Geometries &objects);

	// Search for best axis to cut a given aabb
	SplitAxis	bestNodeCut(const AABB &aabb) const;

	const Geometry*	innerTraverse(BVHNode *node, const Ray &ray, Real &t) const;
	void			innerGather(BVHNode *node, const Ray &ray, Real &t, Geometry **accum, Real *distances, size_t &startIdx, size_t maxObj) const;

private:
	BVHNode *root;
};

#endif
