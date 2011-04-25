#include "convex_polygon.h"

#include <algorithm>

using namespace std;

struct ConvexPolygon::Vertex
{
	Point3	pos;
    Vector3 n;
    Color   cs, os;
};

struct ConvexPolygon::Face
{
    ConvexPolygon *owner;

    size_t  nVertices;
    size_t  *verticesIndex;

    Plane   plane;
    Vector3 *edgeNormals;

    void build(const ConvexPolygon::MeshCreationData &data, size_t currentIndex)
    {
        nVertices = data.vertexPerFaces[currentIndex];
        verticesIndex = memory::allocate<size_t>(nVertices);

        size_t vIdx = 0;
        for_each(verticesIndex, verticesIndex + nVertices, [&] (size_t &v) { v = data.faces[currentIndex][vIdx++]; } );

	    // Get supporting plane
	    //! \todo Search valid points instead of assuming the first 3 are...
        plane = Plane::fromPoints(getVertexAt(verticesIndex[0]).pos,
                                  getVertexAt(verticesIndex[1]).pos,
                                  getVertexAt(verticesIndex[2]).pos);

        // Generate each edges normals & generate default vertex normals
        edgeNormals = memory::allocate<Vector3>(nVertices);
	    for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
	    {
		    const size_t next			= (vIdx + 1) % nVertices;
		    const Vector3 nextToCurrent	= getVertexAt(verticesIndex[next]).pos - getVertexAt(verticesIndex[vIdx]).pos;
            edgeNormals[vIdx]           = cross(normalize(nextToCurrent), plane.n);

            //! \todo average already existing normal (if any)
            getVertexAt(verticesIndex[vIdx]).n = plane.n;
        }
    }

    inline ConvexPolygon::Vertex& getVertexAt(size_t idx)
    {
        return owner->vertices[verticesIndex[idx]];
    }

    bool hit(const Ray &ray, IntersectionInfo &ii)
    {
	    float dist;
        Point3 pointInPlane;
        if (!plane.intersection(ray, dist, pointInPlane) || dist > ray.maxT)
		    return false;

	    // Test point-in-polygon (lazy mode)
	    size_t insideCount = 0;
	    for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
	    {
		    const Vector3 PToCurrent = pointInPlane - getVertexAt(verticesIndex[vIdx]).pos;
		    if (dot(edgeNormals[vIdx], PToCurrent) < 0.f)
			    insideCount++;
	    }
	    if (insideCount < nVertices)
		    return false;

        ////// SUBSUBSUBSUBSUBSUB-OPTIMAL! do refinement once closest poly is found!

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
		    const Vector3 PToCurrent = pointInPlane - getVertexAt(verticesIndex[vIdx]).pos;
		    const Vector3 nextToCurrent	= getVertexAt(verticesIndex[next]).pos - getVertexAt(verticesIndex[vIdx]).pos;
		    const float area		= lengthSqr(cross(nextToCurrent, PToCurrent));
		    const float ntcSqrdLen	= lengthSqr(nextToCurrent);
		    if (area <= 0.000001f * ntcSqrdLen)
		    {
			    // Reset all weights and interpolate between this vertex and the next
			    for (size_t wIdx = 0; wIdx != nVertices; wIdx++)
				    weights[wIdx] = 0.f;

			    const float w = lengthSqr(PToCurrent) / ntcSqrdLen;
			    weights[vIdx] = 1.f - w;
			    weights[next] = w;

			    shouldNormalizeWeights = false;
			    break;
		    }

		    const float cot1		= cotangeant(pointInPlane, getVertexAt(verticesIndex[vIdx]).pos, getVertexAt(verticesIndex[prev]).pos);
		    const float cot2		= cotangeant(pointInPlane, getVertexAt(verticesIndex[vIdx]).pos, getVertexAt(verticesIndex[next]).pos);
		    const float distSqrd	= lengthSqr(pointInPlane - getVertexAt(verticesIndex[vIdx]).pos);
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
        
        ii.point    = pointInPlane;
        ii.normal   = Vector3(0.f);
        ii.cs       = Color(0.f);
	    for (size_t vIdx = 0; vIdx != nVertices; vIdx++)
        {
		        ii.normal  += getVertexAt(verticesIndex[vIdx]).n * weights[vIdx];
                ii.cs      += getVertexAt(verticesIndex[vIdx]).cs * weights[vIdx];
                ii.os      += getVertexAt(verticesIndex[vIdx]).os * weights[vIdx];
        }

	    owner->barCoordProvider.local()->ordered_free(weights, nVertices);

        ray.maxT = dist;
        return true;
    }
};

//----------------------------------------------------------------------
// MeshCreationData

ConvexPolygon::MeshCreationData::MeshCreationData(size_t nVertices, size_t nFaces, ComponentSet components)
    :   vertexCount(nVertices), comps(components), facesCount(nFaces),
        points(nullptr), normals(nullptr),
        cs(nullptr), os(nullptr),
        currAddedFace(0), vertexPerFaces(nullptr), faces(nullptr)
{
    if (vertexCount > 0 && facesCount > 0)
    {
        points = memory::allocate<Point3>(vertexCount);

        vertexPerFaces  = memory::allocate<size_t>(facesCount);
        faces           = memory::allocate<size_t*>(facesCount);

        if (comps.test(HasNormals))
            normals = memory::allocate<Vector3>(vertexCount);

        if (comps.test(HasColors))
            cs = memory::allocate<Color>(vertexCount);

        if (comps.test(HasOpacities))
            os = memory::allocate<Color>(vertexCount);
    }
}

ConvexPolygon::MeshCreationData::~MeshCreationData()
{
    if (vertexCount > 0 && facesCount > 0)
    {
        memory::deallocate<Point3>(points);
        
        memory::deallocate<size_t>(vertexPerFaces);
        for_each(faces, faces + facesCount, &memory::deallocate<size_t>);
        memory::deallocate<size_t*>(faces);

        if (comps.test(HasNormals))
            memory::deallocate<Vector3>(normals);

        if (comps.test(HasColors))
            memory::deallocate<Color>(cs);

        if (comps.test(HasOpacities))
            memory::deallocate<Color>(os);
    }
}

void ConvexPolygon::MeshCreationData::addFace(size_t faceSize, size_t *verticesIdx)
{
    vertexPerFaces[currAddedFace] = faceSize;
    
    faces[currAddedFace] = memory::allocate<size_t>(faceSize);
    for (size_t idx = 0; idx != faceSize; idx++)
        faces[currAddedFace][idx] = verticesIdx[idx];

    currAddedFace++;
}

//----------------------------------------------------------------------
// ConvexPolygon

ConvexPolygon* ConvexPolygon::create(const Matrix4 &obj2world, const MeshCreationData &data)
{
    if (data.vertexCount == 0)
        return nullptr;

    ConvexPolygon *result(memory::construct<ConvexPolygon>(obj2world));

    result->nVertices	= data.vertexCount;
	result->vertices	= memory::allocate<Vertex>(result->nVertices);

	// Copy vertices' positions
	size_t vIdx = 0;
	for_each(data.points, data.points + result->nVertices,
        [&] (const Point3 &p)
        {
            result->vertices[vIdx].pos  = p;
            result->vertices[vIdx].cs   = result->color;
            result->vertices[vIdx].os   = result->opacity;
            vIdx++;
        } );

	result->buildAABB();

    // Build faces infos
    result->nFaces = data.facesCount;
    result->faces = memory::allocate<Face>(result->nFaces);
    size_t idx = 0;
    for_each(result->faces, result->faces + result->nFaces,
        [&] (Face &f)
        {
            f.owner = result;
            f.build(data, idx);
            idx++;
        } );
	
	// Copy additional per-vertex data
	for (vIdx = 0; vIdx != result->nVertices; vIdx++)
	{
        if (data.normals != nullptr)
            result->vertices[vIdx].n = normalize(data.normals[vIdx]);

        if (data.cs != nullptr)
            result->vertices[vIdx].cs = data.cs[vIdx];

        if (data.os != nullptr)
            result->vertices[vIdx].os = data.os[vIdx];
	}

    return result;
}

ConvexPolygon::ConvexPolygon()
	: barCoordProvider(&memory::PoolCreator<float, 1>::create)
{
}

ConvexPolygon::ConvexPolygon(const Matrix4 &obj2world)
	: Geometry(obj2world), barCoordProvider(&memory::PoolCreator<float, 1>::create)
{
}

ConvexPolygon::~ConvexPolygon()
{
	memory::deallocate(vertices);
	memory::deallocate(faces);
}

void ConvexPolygon::buildAABB()
{
	for_each(vertices, vertices + nVertices,
		[&] (const Vertex &v)
		{
			aabb._min = minPerElem(aabb._min, v.pos);
			aabb._max = maxPerElem(aabb._max, v.pos);
		} );
}

bool ConvexPolygon::hit(const Ray &ray, IntersectionInfo &ii) const
{
	Ray localRay(worldToObject * ray);

    // Get closest face
    bool hitAFace = false;
    for (size_t faceIndex = 0; faceIndex != nFaces; faceIndex++)
        hitAFace |= faces[faceIndex].hit(localRay, ii);

	// Fill intersection info
    if (hitAFace)
	    ii.point = Point3((objectToWorld * ii.point).get128());

	return hitAFace;
}
