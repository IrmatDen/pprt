#include <limits>
#include <set>
#include <vector>

#include "bvh.h"

using namespace std;

BVH::~BVH()
{
	clear();
}

void BVH::build(const Scene::Geometries &objects)
{
	root = memory::construct<BVHNode>();
	setAABBFor(root->aabb, objects);

	buildSubTree(*root, objects);
}

void BVH::clear()
{
	memory::destroy(root);
}

void BVH::setAABBFor(AABB &aabb, const Scene::Geometries &objects) const
{
	if (objects.size() == 0)
	{
		aabb = AABB();
		return;
	}

	aabb = objects[0]->getAABB();
	for(Scene::Geometries::const_iterator it = objects.begin() + 1; it != objects.end(); ++it)
		aabb.mergeWith((*it)->getAABB());
}

void BVH::buildSubTree(BVHNode &currentNode, const Scene::Geometries &objects)
{
	if (objects.size() <= BVHNode::MaxObjPerLeaf)
	{
		currentNode.isLeaf		= true;
		currentNode.objCount	= objects.size();
		currentNode.objects		= memory::construct<Geometry*>((size_t)currentNode.objCount);
		int idx = 0;
		for (Scene::Geometries::const_iterator it = objects.begin(); it != objects.end(); ++it, ++idx)
			currentNode.objects[idx] = *it;
		return;
	}

	SplitAxis sa = bestNodeCut(currentNode.aabb, objects);

	Scene::Geometries leftObjects;
	Scene::Geometries rightObjects;
	currentNode.left = memory::construct<BVHNode>();
	currentNode.right = memory::construct<BVHNode>();

	if (sa != SA_None)
		splitObjects(sa, currentNode.aabb, objects, leftObjects, rightObjects, currentNode.left->aabb, currentNode.right->aabb);
	else
	{
		bool pair = true;
		for(Scene::Geometries::const_iterator it = objects.begin(); it != objects.end(); ++it, pair = !pair)
		{
			if (pair)
				leftObjects.push_back(*it);
			else
				rightObjects.push_back(*it);
		}
	}

	buildSubTree(*currentNode.left, leftObjects);
	buildSubTree(*currentNode.right, rightObjects);
}

BVH::SplitAxis BVH::bestNodeCut(const AABB &aabb, const Scene::Geometries &objects) const
{
	Scene::Geometries leftGeo, rightGeo;
	AABB leftAABB, rightAABB;
	float cost, bestCost = numeric_limits<float>::infinity();
	SplitAxis bestCut = SA_None;

	// X test
	splitObjects(SA_X, aabb, objects, leftGeo, rightGeo, leftAABB, rightAABB);
	cost = leftAABB.surfaceArea() * leftGeo.size() + rightAABB.surfaceArea() * rightGeo.size();
	if (cost < bestCost && leftGeo.size() > 0 && rightGeo.size() > 0)
	{
		bestCost	= cost;
		bestCut		= SA_X;
	}

	// Y test
	splitObjects(SA_Y, aabb, objects, leftGeo, rightGeo, leftAABB, rightAABB);
	cost = leftAABB.surfaceArea() * leftGeo.size() + rightAABB.surfaceArea() * rightGeo.size();
	if (cost < bestCost && leftGeo.size() > 0 && rightGeo.size() > 0)
	{
		bestCost	= cost;
		bestCut		= SA_Y;
	}

	// Z test
	splitObjects(SA_Z, aabb, objects, leftGeo, rightGeo, leftAABB, rightAABB);
	cost = leftAABB.surfaceArea() * leftGeo.size() + rightAABB.surfaceArea() * rightGeo.size();
	if (cost < bestCost && leftGeo.size() > 0 && rightGeo.size() > 0)
		bestCut		= SA_Z;

	return bestCut;
}

void BVH::splitObjects(SplitAxis sa, const AABB &aabb, const Scene::Geometries &objects,
					   Scene::Geometries &leftObjects, Scene::Geometries &rightObjects,
					   AABB &leftAABB, AABB &rightAABB
					  ) const
{
	swap(leftObjects, Scene::Geometries());
	leftAABB = AABB();
	swap(rightObjects, Scene::Geometries());
	rightAABB = AABB();

	float cutCoord;
	cutCoord = aabb._min[sa] + (aabb._max[sa] - aabb._min[sa]) / 2;

	for (Scene::Geometries::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		Geometry *g = *it;

		switch(sa)
		{
		case SA_X:
			if (g->position().getX() < cutCoord)
			{
				leftObjects.push_back(g);
				leftAABB.mergeWith(g->getAABB());
			}
			else
			{
				rightObjects.push_back(g);
				rightAABB.mergeWith(g->getAABB());
			}
			break;
			
		case SA_Y:
			if (g->position().getY() < cutCoord)
			{
				leftObjects.push_back(g);
				leftAABB.mergeWith(g->getAABB());
			}
			else
			{
				rightObjects.push_back(g);
				rightAABB.mergeWith(g->getAABB());
			}
			break;
			
		case SA_Z:
			if (g->position().getZ() < cutCoord)
			{
				leftObjects.push_back(g);
				leftAABB.mergeWith(g->getAABB());
			}
			else
			{
				rightObjects.push_back(g);
				rightAABB.mergeWith(g->getAABB());
			}
			break;
		}
	}
}

const Geometry* BVH::findClosest(const Ray &ray, float &t, IntersectionInfo &ii) const
{
	return innerTraverse(root, ray, t, ii);
}

const Geometry* BVH::innerTraverse(BVHNode *node, const Ray &ray, float &t, IntersectionInfo &ii) const
{
	if (!node->aabb.hit(ray, t))
		return 0;

	if (!node->isLeaf)
	{
		const Geometry *lResult(innerTraverse(node->left, ray, t, ii));
		const Geometry *rResult(innerTraverse(node->right, ray, t, ii));
		
		return rResult ? rResult : lResult;
	}

	const Geometry *closest(0);
	for (int i = 0; i < node->objCount; i++)
	{
		if (node->objects[i]->hit(ray, t, ii))
			closest = node->objects[i];
	}
	
	return closest;
}
