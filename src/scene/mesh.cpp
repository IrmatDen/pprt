#include "mesh.h"

#include <algorithm>
#include <limits>

using namespace std;

struct Mesh::Vertex
{
	Point3	pos;
    Vector3 n;
    Color   cs, os;
};

struct Mesh::Face
{
    Mesh *owner;

    size_t  nVertices;
    size_t  *verticesIndex;

    Plane   plane;
    Vector3 *edgeNormals;

    void build(const Mesh::MeshCreationData &data, size_t currentIndex)
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

    inline Mesh::Vertex& getVertexAt(size_t idx)
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
		    const Vector3 PToCurrent = pointInPlane - getVertexAt(vIdx).pos;
		    if (dot(edgeNormals[vIdx], PToCurrent) < 0.001f)
			    insideCount++;
	    }
	    if (insideCount < nVertices)
		    return false;

        ii.P = pointInPlane;
        ray.maxT = dist;
        
        return true;
    }

    void refineHit(IntersectionInfo &ii)
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
		    /* FIXME something's wrong with this method
            const Vector3 PToCurrent = ii.point - getVertexAt(vIdx).pos;
		    const Vector3 nextToCurrent	= getVertexAt(next).pos - getVertexAt(vIdx).pos;
		    const float area		= lengthSqr(cross(nextToCurrent, PToCurrent));
		    const float ntcSqrdLen	= lengthSqr(nextToCurrent);
		    if (area <= numeric_limits<float>::epsilon() * ntcSqrdLen)
		    {
			    // Reset all weights and interpolate between this vertex and the next
			    for (size_t wIdx = 0; wIdx != nVertices; wIdx++)
				    weights[wIdx] = 0.f;

			    const float w = lengthSqr(PToCurrent) / ntcSqrdLen;
			    weights[vIdx] = 1.f - w;
			    weights[next] = w;

			    shouldNormalizeWeights = false;
			    break;
		    }*/

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
        /*if (dotps(ii.normal.get128(), (-ray.direction()).get128()).m128_f32[0] < 0.f)
        {
	        owner->barCoordProvider.local()->ordered_free(weights, nVertices);
            return false;
        }*/

	    owner->barCoordProvider.local()->ordered_free(weights, nVertices);
    }
};

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
    result->faces = memory::allocate<Face>(result->nFaces);
    size_t idx = 0;
    for_each(result->faces, result->faces + result->nFaces,
        [&] (Face &f)
        {
            f.owner = result;
            f.build(data, idx);
            idx++;
        } );

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
	memory::deallocate(faces);
}

void Mesh::buildAABB()
{
	for_each(vertices, vertices + nVertices,
		[&] (const Vertex &v)
		{
			aabb._min = minPerElem(aabb._min, v.pos);
			aabb._max = maxPerElem(aabb._max, v.pos);
		} );
}

bool Mesh::hit(const Ray &ray, IntersectionInfo &ii) const
{
	Ray localRay(worldToObject * ray);

    // Get closest face
    Face *closestFace = nullptr;
    for (size_t faceIndex = 0; faceIndex != nFaces; faceIndex++)
    {
        if (faces[faceIndex].hit(localRay, ii))
            closestFace = &faces[faceIndex];
    }

	// Fill intersection info
    if (closestFace == nullptr)
        return false;

    closestFace->refineHit(ii);
	ii.P  = Point3((objectToWorld * ii.P).get128());
	ii.N  = Vector3((worldToObjectN * ii.N).get128());
	ii.Ng = Vector3((worldToObjectN * ii.Ng).get128());
	return true;
}
