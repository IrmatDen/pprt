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

	void		build(const Scene::Geometries &objects);

	Geometry*	traverse(const Ray &ray, Real &t) const;

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

private:
	void		buildTwoObjMax(const Scene::Geometries &objects);
	void		genericBuild(const Scene::Geometries &objects);

	void		buildLeafNodes(const Scene::Geometries &objects, BVHNode *leafNodes, size_t &leafNodesCount);

private:
	BVHNode *root;
};

#endif
