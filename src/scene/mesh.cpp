#include "mesh.h"

#include <algorithm>
#include <limits>

using namespace std;

struct Mesh::Triangle : public Mesh::Face
{
    virtual bool hit(const Ray &ray, IntersectionInfo &ii) const
	{
	}

    virtual void refineHit(IntersectionInfo &ii) const
	{
	}
};

inline Mesh::Vertex& Mesh::Face::getVertexAt(size_t idx) const
{
    return owner->vertices[verticesIndex[idx]];
}

AABB Mesh::Face::getAABB() const
{
    // Yeah, I'm doing it on the fly... Bigger mesh's bvh build times, but smaller Face, so...
    AABB ret;
    ret._min = ret._max = getVertexAt(0).pos;
    for (size_t i = 1; i != nVertices; i++)
    {
		ret._min = minPerElem(ret._min, getVertexAt(i).pos);
		ret._max = maxPerElem(ret._max, getVertexAt(i).pos);
    }
    
    return ret;
}

Point3 Mesh::Face::position() const
{
    // Same thing here: on the fly.
    Point3 ret(getVertexAt(0).pos);
    for (size_t i = 1; i != nVertices; i++)
		ret += Vector3(getVertexAt(i).pos);
    ret = Point3(Vector3(ret) * (1 / static_cast<float>(nVertices)));
    
    return ret;
}

void Mesh::Face::build(const Mesh::MeshCreationData &data, size_t currentIndex)
{
    nVertices = data.vertexPerFaces[currentIndex];
    verticesIndex = memory::allocate<size_t>(nVertices);

    size_t vIdx = 0;
    for_each(verticesIndex, verticesIndex + nVertices, [&] (size_t &v) { v = data.faces[currentIndex][vIdx++]; } );

	// Get supporting plane
	//! \todo Search valid points instead of assuming the first 3 are...
    plane = Plane::fromPoints(getVertexAt(0).pos,
                              getVertexAt(1).pos,
                              getVertexAt(2).pos);

    // Generate each edges normals & generate default vertex normals
    edgeNormals = memory::allocate<Vector3>(nVertices);
	for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
	{
		const size_t next			= (vIdx + 1) % nVertices;
        Vertex &currentVertex       = getVertexAt(vIdx);
        const Vertex &nextVertex    = getVertexAt(next);

		const Vector3 nextToCurrent	= nextVertex.pos - currentVertex.pos;
        edgeNormals[vIdx]           = cross(normalize(nextToCurrent), plane.n);

        if (lengthSqr(currentVertex.n) < 0.001)
            currentVertex.n = plane.n;
        else
            currentVertex.n += plane.n;
    }
}

bool Mesh::Face::hit(const Ray &ray, IntersectionInfo &ii) const
{
    // Check orientation
    if (dot(plane.n, ray.direction()) > 0)
        return false;

    // Check & get point in polygon's plane
	float dist;
    Point3 pointInPlane;
    if (!plane.intersection(ray, dist, pointInPlane) || dist > ray.maxT)
		return false;

	// Test point-in-polygon (lazy mode)
	size_t insideCount = 0;
	for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
	{
		const Vector3 PToCurrent = pointInPlane - getVertexAt(vIdx).pos;
#ifdef SSE4
		if (dotps(edgeNormals[vIdx].get128(), PToCurrent.get128()).m128_f32[0] > -0.000001f)
#else
		if (dot(edgeNormals[vIdx], PToCurrent) > 0.001f)
#endif
			return false;
	}

    ii.P = pointInPlane;
    ray.maxT = dist;
        
    return true;
}

void Mesh::Face::refineHit(IntersectionInfo &ii) const
{
	// Compute barycentric coordinates based on
	// "Generalized Barycentric Coordinates on Irregular Polygons"
	// by Meyer et al. (2002)
	// Published in JGT Vol. 7, Nr 1, 2002
	float *weights	= reinterpret_cast<float*>(owner->barCoordProvider.local()->ordered_malloc(nVertices));
	float weightSum	= 0.f;
    bool shouldNormalizeWeights = true;
	for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
	{
		const size_t prev	= (vIdx + nVertices - 1) % nVertices;
		const size_t next	= (vIdx + 1) % nVertices;
 
		// Determine if point is almost on an edge
		const Vector3 PToCurrent = ii.P - getVertexAt(vIdx).pos;
		const Vector3 nextToCurrent	= getVertexAt(next).pos - getVertexAt(vIdx).pos;
		const float area		= lengthSqr(cross(nextToCurrent, PToCurrent));
		const float ntcSqrdLen	= lengthSqr(nextToCurrent);
		if (area <= numeric_limits<float>::epsilon() * ntcSqrdLen)
		{
			// Reset all weights and interpolate between this vertex and the next
			for (size_t wIdx = 0; wIdx != nVertices; wIdx++)
				weights[wIdx] = 0.f;

			const float w = length(PToCurrent) / sqrt(ntcSqrdLen);
			weights[vIdx] = 1.f - w;
			weights[next] = w;

			shouldNormalizeWeights = false;
			break;
		}

		const float cot1		= cotangeant(ii.P, getVertexAt(vIdx).pos, getVertexAt(prev).pos);
		const float cot2		= cotangeant(ii.P, getVertexAt(vIdx).pos, getVertexAt(next).pos);
		const float distSqrd	= lengthSqr(ii.P - getVertexAt(vIdx).pos);
		weights[vIdx]			= (cot1 + cot2) / distSqrd;
		weightSum				+= weights[vIdx];
	}
	
	// Normalize weights
    if (shouldNormalizeWeights)
    {
		const float invWeightSum = 1.f / weightSum;
		for (size_t wIdx = 0; wIdx != nVertices; wIdx++)
			weights[wIdx] *= invWeightSum;
    }
    
    ii.N   = Vector3(0.f);
    ii.Cs  = Color(0.f);
    ii.Os  = Color(0.f);
	for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
    {
        if (owner->vertexFormat.test(Mesh::HasNormals))
		    ii.N  += getVertexAt(vIdx).n * weights[vIdx];
        ii.Cs      += getVertexAt(vIdx).cs * weights[vIdx];
        ii.Os      += getVertexAt(vIdx).os * weights[vIdx];
    }
    ii.Ng = plane.n;

    if (!owner->vertexFormat.test(Mesh::HasNormals))
        ii.N = ii.Ng;
    else
        ii.N = normalize(ii.N);

    if (!owner->vertexFormat.test(Mesh::HasColors))
        ii.Cs = owner->color;
    if (!owner->vertexFormat.test(Mesh::HasOpacities))
        ii.Os = owner->opacity;

	owner->barCoordProvider.local()->ordered_free(weights, nVertices);
}

//----------------------------------------------------------------------
// MeshCreationData

Mesh::MeshCreationData::MeshCreationData(size_t nVertices, size_t nFaces, ComponentSet components)
    :   vertexCount(nVertices), vertexFormat(components), facesCount(nFaces),
        points(nullptr), normals(nullptr),
        cs(nullptr), os(nullptr),
        currAddedFace(0), vertexPerFaces(nullptr), faces(nullptr)
{
    if (vertexCount > 0 && facesCount > 0)
    {
        points = memory::allocate<Point3>(vertexCount);

        vertexPerFaces  = memory::allocate<size_t>(facesCount);
        faces           = memory::allocate<size_t*>(facesCount);

        if (vertexFormat.test(HasNormals))
            normals = memory::allocate<Vector3>(vertexCount);

        if (vertexFormat.test(HasColors))
            cs = memory::allocate<Color>(vertexCount);

        if (vertexFormat.test(HasOpacities))
            os = memory::allocate<Color>(vertexCount);
    }
}

Mesh::MeshCreationData::~MeshCreationData()
{
    if (vertexCount > 0 && facesCount > 0)
    {
        memory::deallocate<Point3>(points);
        
        memory::deallocate<size_t>(vertexPerFaces);
        for_each(faces, faces + facesCount, &memory::deallocate<size_t>);
        memory::deallocate<size_t*>(faces);

        if (vertexFormat.test(HasNormals))
            memory::deallocate<Vector3>(normals);

        if (vertexFormat.test(HasColors))
            memory::deallocate<Color>(cs);

        if (vertexFormat.test(HasOpacities))
            memory::deallocate<Color>(os);
    }
}

void Mesh::MeshCreationData::addFace(size_t faceSize, const size_t * const verticesIdx)
{
    vertexPerFaces[currAddedFace] = faceSize;
    
    faces[currAddedFace] = memory::allocate<size_t>(faceSize);
    for (size_t idx = 0; idx != faceSize; idx++)
        faces[currAddedFace][idx] = verticesIdx[idx];

    currAddedFace++;
}

//----------------------------------------------------------------------
// Mesh

Mesh* Mesh::create(Scene *scn, const Matrix4 &obj2world, const MeshCreationData &data)
{
    if (data.vertexCount == 0)
        return nullptr;

    Mesh *result(memory::construct<Mesh>(scn, obj2world));

    result->vertexFormat = data.vertexFormat;

    result->nVertices	= data.vertexCount;
	result->vertices	= memory::allocate<Vertex>(result->nVertices);

	// Copy vertices' positions
	size_t vIdx = 0;
	for_each(data.points, data.points + result->nVertices,
        [&] (const Point3 &p)
        {
            result->vertices[vIdx].pos  = p;
            result->vertices[vIdx].n    = Vector3(all_zero());
            result->vertices[vIdx].cs   = result->color;
            result->vertices[vIdx].os   = result->opacity;
            vIdx++;
        } );

	result->buildAABB();

    // Build faces infos
    result->nFaces = data.facesCount;
    result->facesPool = memory::construct<Face>(result->nFaces);
    Face *facePoolEntry = result->facesPool;
    for (size_t fIdx = 0; fIdx != result->nFaces; fIdx++, facePoolEntry++)
    {
        Face *newFace = facePoolEntry;
        newFace->owner = result;
        newFace->build(data, fIdx);

        result->faces.push_back(newFace);
    }

    // Finalize default per-vertex normals & copy additional per-vertex data
	for (vIdx = 0; vIdx != result->nVertices; vIdx++)
	{
        if (data.normals != nullptr)
            result->vertices[vIdx].n = normalize(data.normals[vIdx]);
        else
            // FIXME shared vertices!
            result->vertices[vIdx].n = normalize(result->vertices[vIdx].n * 0.333333334f);

        if (data.cs != nullptr)
            result->vertices[vIdx].cs = data.cs[vIdx];

        if (data.os != nullptr)
            result->vertices[vIdx].os = data.os[vIdx];
	}

    return result;
}

Mesh::Mesh()
	: barCoordProvider(&memory::PoolCreator<float, 1>::create)
{
}

Mesh::Mesh(Scene *scn, const Matrix4 &obj2world)
	: Geometry(scn, obj2world), barCoordProvider(&memory::PoolCreator<float, 1>::create)
{
}

Mesh::~Mesh()
{
	memory::deallocate(vertices);
	memory::deallocate(facesPool);
}

void Mesh::buildAABB()
{
	for_each(vertices, vertices + nVertices,
		[&] (const Vertex &v)
		{
			aabb._min = minPerElem(aabb._min, v.pos);
			aabb._max = maxPerElem(aabb._max, v.pos);
		} );
    aabb._min = Point3((objectToWorld * aabb._min).get128());
    aabb._max = Point3((objectToWorld * aabb._max).get128());
}

void Mesh::onPrepare()
{
    bvh.build(faces);
}

bool Mesh::hit(const Ray &ray, IntersectionInfo &ii) const
{
	Ray localRay(worldToObject * ray);

    IntersectionInfo proxyInfo;

    // Get closest face
    const Face *closestFace = bvh.findClosest(localRay, proxyInfo);

	// Fill intersection info
    if (closestFace == nullptr)
        return false;

    ii = proxyInfo;
    closestFace->refineHit(ii);
	ii.P  = Point3((objectToWorld * ii.P).get128());
	ii.N  = Vector3((worldToObjectN * ii.N).get128());
	ii.Ng = Vector3((worldToObjectN * ii.Ng).get128());
	return true;
}
