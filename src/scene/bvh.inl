template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf>
BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::BVH()
    : root(0)
{
}

template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf>
BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::~BVH()
{
	clear();
}

template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf>
void BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::build(const ObjectContainer &objects)
{
	root = memory::construct<BVHNode>();
	setAABBFor(root->aabb, objects);

	buildSubTree(*root, objects);
}

template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf>
void BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::clear()
{
	memory::destroy(root);
}

template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf>
void BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::setAABBFor(AABB &aabb, const ObjectContainer &objects) const
{
	if (objects.size() == 0)
	{
		aabb = AABB();
		return;
	}

	aabb = objects.at(0)->getAABB();
	for(ObjectContainer::const_iterator it = objects.begin() + 1; it != objects.end(); ++it)
		aabb.mergeWith((*it)->getAABB());
}

template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf>
void BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::buildSubTree(BVHNode &currentNode, const ObjectContainer &objects)
{
	if (objects.size() <= MaxObjectsPerLeaf)
	{
		currentNode.isLeaf		= true;
		currentNode.objCount	= objects.size();
		currentNode.objects		= memory::construct<Object*>((size_t)currentNode.objCount);
		int idx = 0;
		for (ObjectContainer::const_iterator it = objects.begin(); it != objects.end(); ++it, ++idx)
			currentNode.objects[idx] = *it;
		return;
	}

	SplitAxis sa = bestNodeCut(currentNode.aabb, objects);

	ObjectContainer leftObjects;
	ObjectContainer rightObjects;
	currentNode.left = memory::construct<BVHNode>();
	currentNode.right = memory::construct<BVHNode>();

	if (sa != SA_None)
		splitObjects(sa, currentNode.aabb, objects, leftObjects, rightObjects, currentNode.left->aabb, currentNode.right->aabb);
	else
	{
		bool pair = true;
		for(ObjectContainer::const_iterator it = objects.begin(); it != objects.end(); ++it, pair = !pair)
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

template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf>
typename BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::SplitAxis
BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::bestNodeCut(const AABB &aabb, const ObjectContainer &objects) const
{
	ObjectContainer leftGeo, rightGeo;
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

template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf>
void BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::splitObjects(SplitAxis sa, const AABB &aabb, const ObjectContainer &objects,
					   ObjectContainer &leftObjects, ObjectContainer &rightObjects,
					   AABB &leftAABB, AABB &rightAABB
					  ) const
{
	swap(leftObjects, ObjectContainer());
	leftAABB = AABB();
	swap(rightObjects, ObjectContainer());
	rightAABB = AABB();

	float cutCoord;
	cutCoord = aabb._min[sa] + (aabb._max[sa] - aabb._min[sa]) / 2;

	for (ObjectContainer::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		Object *g = *it;

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

template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf>
const Object* BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::findClosest(const Ray &ray, IntersectionInfo &ii) const
{
	float tmin, tmax;
	return innerTraverse(root, ray, tmin, tmax, ii);
}

template<typename Object, typename ObjectContainer, int MaxObjectsPerLeaf>
const Object* BVH<Object, ObjectContainer, MaxObjectsPerLeaf>::innerTraverse(BVHNode *node, const Ray &ray, float &tmin, float &tmax, IntersectionInfo &ii) const
{
	float thisMin, thisMax;
	if (!node->aabb.hit(ray, thisMin, thisMax) || ray.maxT < thisMin || ray.minT > thisMax)
		return 0;

	tmin = thisMin;
	tmax = thisMax;

	if (!node->isLeaf)
	{
		const Object *lResult(innerTraverse(node->left, ray, tmin, tmax, ii));
		const Object *rResult(innerTraverse(node->right, ray, tmin, tmax, ii));
		
		return rResult ? rResult : lResult;
	}

	const Object *closest(0);
	for (int i = 0; i < node->objCount; i++)
	{
		if (node->objects[i]->hit(ray, ii))
		{
			tmax = ray.maxT;
			closest = node->objects[i];
		}
	}
	
	return closest;
}
