#include <limits>
#include <set>
#include <vector>

#include "bvh.h"

using namespace std;

BVH::~BVH()
{
	delete root;
}

void BVH::build(const Scene::Geometries &objects)
{
	root = new BVHNode();
	setAABBFor(root->aabb, objects);

	buildSubTree(*root, objects);
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
	if (objects.size() == 0)
	{
		currentNode.isLeaf		= true;
		return;
	}
	if (objects.size() == 1)
	{
		currentNode.isLeaf		= true;
		currentNode.geoLeft		= objects[0].get();
		return;
	}
	if (objects.size() == 2)
	{
		currentNode.geoLeft		= objects[0].get();
		currentNode.geoRight	= objects[1].get();
		currentNode.isLeaf		= true;
		return;
	}

	SplitAxis sa = bestNodeCut(currentNode.aabb, objects);

	Scene::Geometries leftObjects;
	Scene::Geometries rightObjects;

	currentNode.left = new BVHNode();
	currentNode.right = new BVHNode();

	splitObjects(sa, currentNode.aabb, objects, leftObjects, rightObjects, currentNode.left->aabb, currentNode.right->aabb);

	buildSubTree(*currentNode.left, leftObjects);
	buildSubTree(*currentNode.right, rightObjects);
}

BVH::SplitAxis BVH::bestNodeCut(const AABB &aabb, const Scene::Geometries &objects) const
{
	Scene::Geometries leftGeo, rightGeo;
	AABB leftAABB, rightAABB;
	Real cost, bestCost = numeric_limits<Real>::infinity();
	SplitAxis bestCut = (SplitAxis)-1;

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

	Real cutCoord;
	if (sa == SA_X)
		cutCoord = aabb._min.x + (aabb._max.x - aabb._min.x) / 2;
	else if (sa == SA_Y)
		cutCoord = aabb._min.y + (aabb._max.y - aabb._min.y) / 2;
	else // cut axis == Z
		cutCoord = aabb._min.z + (aabb._max.z - aabb._min.z) / 2;

	for (Scene::Geometries::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		GeometryPtr g = *it;

		switch(sa)
		{
		case SA_X:
			if (g->position().x < cutCoord)
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
			if (g->position().y < cutCoord)
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
			if (g->position().z < cutCoord)
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

const Geometry* BVH::findClosest(const Ray &ray, Real &t) const
{
	return innerTraverse(root, ray, t);
}

const Geometry* BVH::innerTraverse(BVHNode *node, const Ray &ray, Real &t) const
{
	if (!node->aabb.hit(ray, t))
		return 0;

	if (!node->isLeaf)
	{
		const Geometry *lResult(innerTraverse(node->left, ray, t));
		const Geometry *rResult(innerTraverse(node->right, ray, t));
		
		return rResult ? rResult : lResult;
	}

	const Geometry *closest(0);
	if (node->geoLeft)
	{
		if (node->geoLeft->hit(ray, t))
			closest = node->geoLeft;

		if (node->geoRight && node->geoRight->hit(ray, t))
			closest = node->geoRight;
	}
	
	return closest;
}

size_t BVH::gatherAlong(const Ray &ray, Real &t, Geometry **accum, Real *distances, size_t maxObj) const
{
	size_t start = 0;
	innerGather(root, ray, t, accum, distances, start, maxObj);
	return start;
}

void BVH::innerGather(BVHNode *node, const Ray &ray, Real &t, Geometry **accum, Real *distances, size_t &startIdx, size_t maxObj) const
{
	if (!node->aabb.hit(ray, t) || startIdx == maxObj)
		return;

	if (!node->isLeaf)
	{
		innerGather(node->left, ray, t, accum, distances, startIdx, maxObj);
		innerGather(node->right, ray, t, accum, distances, startIdx, maxObj);
		return;
	}

	if (node->geoLeft)
	{
		Real to = t;
		if (node->geoLeft->hit(ray, to))
		{
			accum[startIdx] = node->geoLeft;
			distances[startIdx++] = to;
		}

		if (node->geoRight && startIdx < maxObj)
		{
			to = t;
			if (node->geoRight->hit(ray, t))
			{
				accum[startIdx] = node->geoRight;
				distances[startIdx++] = to;
			}
		}
	}
}
