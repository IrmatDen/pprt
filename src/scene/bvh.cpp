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

	SplitAxis sa = bestNodeCut(currentNode.aabb);

	Scene::Geometries leftObjects;
	Scene::Geometries rightObjects;

	for (Scene::Geometries::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		GeometryPtr g = *it;
		Real cutCoord;

		switch(sa)
		{
		case SA_X:
			cutCoord = currentNode.aabb._min.x + (currentNode.aabb._max.x - currentNode.aabb._min.x) / 2;
			if (g->position().x < cutCoord)
				leftObjects.push_back(g);
			else
				rightObjects.push_back(g);
			break;
			
		case SA_Y:
			cutCoord = currentNode.aabb._min.y + (currentNode.aabb._max.y - currentNode.aabb._min.y) / 2;
			if (g->position().y < cutCoord)
				leftObjects.push_back(g);
			else
				rightObjects.push_back(g);
			break;
			
		case SA_Z:
			cutCoord = currentNode.aabb._min.z + (currentNode.aabb._max.z - currentNode.aabb._min.z) / 2;
			if (g->position().z < cutCoord)
				leftObjects.push_back(g);
			else
				rightObjects.push_back(g);
			break;
		}
	}

	currentNode.left = new BVHNode();
	setAABBFor(currentNode.left->aabb, leftObjects);
	
	currentNode.right = new BVHNode();
	setAABBFor(currentNode.right->aabb, rightObjects);

	buildSubTree(*currentNode.left, leftObjects);
	buildSubTree(*currentNode.right, rightObjects);
}

BVH::SplitAxis BVH::bestNodeCut(const AABB &aabb) const
{
	SplitAxis bestCut = SA_X;

	AABB tmp(aabb);
	tmp._max.x		-= (tmp._max.x - tmp._min.x) / 2;
	Real bestCutCost = tmp.surfaceArea();

	tmp			= aabb;
	tmp._max.y -= (tmp._max.y - tmp._min.y) / 2;
	Real sa		= tmp.surfaceArea();
	if (sa < bestCutCost)
	{
		bestCutCost	= sa;
		bestCut		= SA_Y;
	}

	tmp			= aabb;
	tmp._max.z -= (tmp._max.z - tmp._min.z) / 2;
	sa			= tmp.surfaceArea();
	if (sa < bestCutCost)
	{
		bestCutCost	= sa;
		bestCut		= SA_Z;
	}

	return bestCut;
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
