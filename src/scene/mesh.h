#ifndef PPRT_MESH_H
#define PPRT_MESH_H

#include "geometry.h"
#include "../memory.h"
#include "../math/plane.h"

#include <bitset>

class _MM_ALIGN16 Mesh : public Geometry
{
public:
    enum Components
    {
        HasNormals      = 0,
        HasColors       = 1,
        HasOpacities    = 2,

        ComponentsCount 
    };

    typedef std::bitset<ComponentsCount>  ComponentSet;

public:
    struct MeshCreationData
    {
        MeshCreationData(size_t nVertices, size_t nFaces, ComponentSet components);
        ~MeshCreationData();

        void addFace(size_t faceSize, size_t *verticesIdx);

        ComponentSet vertexFormat;

        Point3  *points;
        Vector3 *normals;
        Color   *cs;
        Color   *os;

    private:
        friend class Mesh;

        MeshCreationData()      { ; }

        size_t  vertexCount;

        size_t  facesCount;
        size_t  currAddedFace;
        size_t  *vertexPerFaces;    //!< Array reporting how many vertices per face for each face
        size_t  **faces;            //!< Array of array of vertex indices, keep null to autogenerate
    };

public:
    static Mesh* create(const Matrix4 &obj2world, const MeshCreationData &data);

public:
	Mesh();
	Mesh(const Matrix4 &obj2world);
	~Mesh();

	virtual bool hit(const Ray &ray, IntersectionInfo &ii) const;

private:
	struct Vertex;
	struct Face;

    friend struct Face;

private:
	void buildAABB();

private:
    ComponentSet    vertexFormat;

	size_t		nVertices;
	Vertex	*	vertices;

    size_t      nFaces;
    Face    *   faces;

	// TLS for barycentric coordinates
	mutable memory::TLPool	barCoordProvider;
};

#endif
