#include <set>

#include "bvh.h"

BVH::~BVH()
{
	delete root;
}

void BVH::build(const Scene::Geometries &objects)
{
	if (objects.size() <= 2)
		buildTwoObjMax(objects);
	else
		genericBuild(objects);
}

void BVH::buildTwoObjMax(const Scene::Geometries &objects)
{
	root = new BVHNode();
	root->isLeaf = true;

	if (objects.size() >= 1)
		root->geoLeft = objects[0].get();

	if(objects.size() == 2)
		root->geoRight = objects[1].get();
}

void BVH::genericBuild(const Scene::Geometries &objects)
{
	// Construct leaf nodes
	BVHNode *	leafNodes(0);
	size_t		leafNodesCount;

	buildLeafNodes(objects, leafNodes, leafNodesCount);
}

void BVH::buildLeafNodes(const Scene::Geometries &objects, BVHNode *leafNodes, size_t &leafNodesCount)
{
	std::set<GeometryPtr> freeObjects;
	for(Scene::Geometries::const_iterator it = objects.begin();
		it != objects.end(); ++it)
	{
		freeObjects.insert(*it);
	}

	leafNodesCount = (size_t)ceil(objects.size() / 2.f);
	leafNodes = new BVHNode[leafNodesCount];

	BVHNode *currentLeafNode = leafNodes;
	for(Scene::Geometries::const_iterator it = objects.begin();
		it != objects.end(); ++it, ++currentLeafNode)
	{
		if (freeObjects.find(*it) != freeObjects.end())
		{
			freeObjects.erase(*it);

			// Find closest non visited geometry's AABB.
			GeometryPtr closest;
			Real minDist = -log(0.f);

			for(std::set<GeometryPtr>::iterator oit = freeObjects.begin();
				oit != freeObjects.end(); ++oit)
			{
				const Vec3 dir = (*oit)->position() - (*it)->position();
				const Real dist = dir.length();

				if (dist < minDist)
				{
					minDist = dist;
					closest = *oit;
				}
			}

			// Build the leaf node.
			currentLeafNode->isLeaf		= true;
			currentLeafNode->geoLeft	= it->get();
			currentLeafNode->geoRight	= closest.get();

			if (closest)
			{
				freeObjects.erase(closest);
				currentLeafNode->aabb.mergeFrom((*it)->getAABB(), closest->getAABB());
			}
		}
	}
}

Geometry* BVH::traverse(const Ray &ray, Real &t) const
{
	return 0;
}
