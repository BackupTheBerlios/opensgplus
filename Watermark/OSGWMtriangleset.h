#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#ifndef _TRIANGLESET_H_
#define _TRIANGLESET_H_

#include <math.h>

//#include <fstream.h>
//#include <fstream>
//#include "./memdebug/memdebug.h"
#include "OSGWMcommontypes.h"

//#include "limits.h"
#include "OSGWMtinyvector.h"
#include "OSGWMtinymatrix.h"
#include "OSGWMarray.h"

#ifndef POWER_CHUNK_SIZE_MAX_ELEMENTS
#define MAX2(x, y)  ((x > y) ? x : y)
#define POWER_CHUNK_SIZE_MAX_ELEMENTS(X) \
        (MAX2(MIN_CHUNK_POWER, (int) ceil(log(X / MAX_NUM_CHUNKS) / log(2.))))
#define POWER_CHUNK_SIZE_MAX_BITS(X)                                        \
        (                                                                   \
            MAX2(MIN_CHUNK_POWER,                                           \
                 (int) MAX2(ceil(log(X / MAX_NUM_CHUNKS) / log(2.)) - 3., 0.)) \
        )
#endif
#define IMPORT_FILE_TYPE_VRS    1
#define IMPORT_FILE_TYPE_OBJ    2
#define IMPORT_FILE_TYPE_OFF    3

/*
  Re-Implementation of MAM/VRS class RTriangleSet.
  Mimics interface but no source code of implementation used.

  Optimized with respect to memory consumption.

  NO growing of a triangle set (after import or init) is supported !
*/
#define VEC2DFORMAT_FIG31   0
#define VEC2DFORMAT_EPS     1

typedef struct
{
    TinyVector  urb;
    TinyVector  llf;
    double      diameter;
} BoundingBoxInfo;

typedef struct
{
    int ind[3];
} TriangleFace;

typedef struct
{
    TinyVector  v[3];
} TriangleVertices;

/* Vertices in edge-descriptions have to be sorted: ind[0] < ind[1] !! */
typedef struct
{
    int ind[2];

    /* more attributes ... */
} EdgeInfo;

typedef struct
{
    int ind[2];
} GEOMARK_Edge;

/* macros */
#define GET_TRIANGLE_VERTICES(TRID, VARRAY) \
    VARRAY[0] = faces_[TRID].ind[0];        \
    VARRAY[1] = faces_[TRID].ind[1];        \
    VARRAY[2] = faces_[TRID].ind[2]

#define IS_FACE_VERTEX(TS, TRID, VID)           \
        (                                       \
            (TS->faces_[TRID].ind[0] == VID) || \
            (TS->faces_[TRID].ind[1] == VID) || \
            (TS->faces_[TRID].ind[2] == VID)    \
        )

/* triangle selection */
/* take all triangles regardless of properties */
#define TRIANGLE_SEL_ALL    0x0000

/* no triangles with 2 or 3 equal vertices (indices) */
#define TRIANGLE_SEL_NO_DEGEN   0x0001

/* no triangles with same vertices (points) */
#define TRIANGLE_SEL_NO_DOUBLE_TRIDS    0x0002

/* no triangle with 3 points on a line (points) */
#define TRIANGLE_SEL_NO_THREE_POINTS_ON_LINE    0x0004

typedef struct
{
    int triangleId;
    int vertexId;
}
SortTrianglesByVertexIndicesEntry;

typedef struct
{
    int     triangleId;
    double  comZ;
} SortTrianglesByComZ;

typedef struct
{
    int     angleId;
    double  angle;
} SortAngles;

typedef struct
{
    int startIndex;
    int lastIndex;
} AdjTrianglesIndex;

typedef struct
{
    double  edgeMinLength;
    double  lolimit, uplimit;                           /* lower and upper limit for
						    edge length ratios */
    double  minAngle;                                   /* in radians */
} DegenSpec;

class   TriangleSet
{
private:

    double              bbDiameter_;
    TinyVector          llf_, urb_;                     /* lower left front and upper right back points
					    of bounding box */

    GEOMARK_BitArray    *doubleTriangle_[2];            /* used internally in getAdjacentTriangles() */
    GrowArray           *tmpTriangleList_[4];           /* index 0 is used internally in getAdjacentTriangles() 
                                     index 1 "                     generateAdjList_()
	                                 index 1 "                     getAdjacentVertices()
                                     index 1 "                     randomizeFaceOrder()
                                     index 1-3 "                   getAdjacentTrianglesFromTriangle()
									 index 1 "                     vertexNormal()
									 index 2 "					   isBorderVertex()
									 index 2 "                     reportDegeneracies()
                                    */
    GrowArray           *tmpVertexList_;                /* used internally in setNumAdjVertices_(), isEdge() */

    GEOMARK_BitArray    *verticesCopied_;               /* used internally in getAdjacentVertices() */
    GEOMARK_BitArray    *verticesMarked_[1];            /* index 0 is used internally in isBorderVertex() */

    void                sortTriangles_(void);
    void                generateAdjList_(void);

    void                getAdjTriangles_(int vertexId,
                                         GrowArray *adjTrianglesList);
    void                getAdjTriangles2_(int vertexId,
                                          GrowArray *adjTrianglesList);

    void                setNumAdjVertices_(void);

    /* most vars are public since we want direct population
    of the set from the outside */
public:

    int                                 debug_;

    int                                 np_, nf_;
    int                                 numPatterns_;
    TriangleFace                        *faces_;
    TinyVector                          **points_;
    BOOL                                dataComplete_;

    GrowArray                           *adjTriangles_; /* type: int */
    GrowArray                           *adjTrianglesIndex_;                        /* type: AdjTrianglesIndex */

    int                                 *numAdjVertices_;

    SortTrianglesByVertexIndicesEntry   *trianglesSorted_[3];                       /* triangles sorted in ascending order based
                            on their first, second and third vertex index */

    HashArray                           *triangleHashTable_;                        /* gives same values for triangles
                                  with same vertices */
    GEOMARK_BitArray                    *triangleHashMarked_;                       /* used in getAdjacentTrianglesFromTriangle() and
                                           isBorderVertex() */

    BOOL                                boundingBoxCalculated_;

    TriangleSet(void)
    {
        int i;

        debug_ = 0;
        np_ = 0;
        nf_ = 0;
        faces_ = NULL;
        triangleHashTable_ = NULL;
        triangleHashMarked_ = NULL;
        numPatterns_ = 0;
        for(i = 0; i < 3; i++)
            trianglesSorted_[i] = NULL;
        adjTriangles_ = NULL;
        adjTrianglesIndex_ = NULL;
        for(i = 0; i < 2; i++)
            doubleTriangle_[i] = NULL;
        for(i = 0; i < 4; i++)
            tmpTriangleList_[i] = NULL;
        tmpVertexList_ = NULL;
        verticesCopied_ = NULL;
        for(i = 0; i < 1; i++)
            verticesMarked_[i] = NULL;
        dataComplete_ = FALSE;
        boundingBoxCalculated_ = FALSE;
        numAdjVertices_ = NULL;
    }

    ~TriangleSet(void)
    {
        int i;

        if(nf_ > 0)
        {
            GEOMARK_FREE(faces_);
            faces_ = NULL;
        }
        if(np_ > 0)
        {
            for(i = 0; i < np_; i++)
            {
                GEOMARK_DELETE(points_[i]);
                points_[i] = NULL;
            }
            GEOMARK_FREE(points_);
            points_ = NULL;
        }
        if(triangleHashTable_)
        {
            //delete(triangleHashTable_);
            GEOMARK_DELETE(triangleHashTable_);
            triangleHashTable_ = NULL;
        }
        if(triangleHashMarked_)
        {
            GEOMARK_DELETE(triangleHashMarked_);
            triangleHashMarked_ = NULL;
        }
        for(i = 0; i < 3; i++)
        {
            if(trianglesSorted_[i])
            {
                GEOMARK_FREE(trianglesSorted_[i]);
                trianglesSorted_[i] = NULL;
            }
        }
        if(adjTriangles_)
        {
            GEOMARK_DELETE(adjTriangles_);
            adjTriangles_ = NULL;
        }
        if(adjTrianglesIndex_)
        {
            GEOMARK_DELETE(adjTrianglesIndex_);
            adjTrianglesIndex_ = NULL;
        }
        for(i = 0; i < 2; i++)
        {
            if(doubleTriangle_[i])
            {
                GEOMARK_DELETE(doubleTriangle_[i]);
                doubleTriangle_[i] = NULL;
            }
        }
        for(i = 0; i < 4; i++)
        {
            if(tmpTriangleList_[i])
            {
                GEOMARK_DELETE(tmpTriangleList_[i]);
                tmpTriangleList_[i] = NULL;
            }
        }
        if(tmpVertexList_)
        {
            GEOMARK_DELETE(tmpVertexList_);
            tmpVertexList_ = NULL;
        }
        if(verticesCopied_)
        {
            GEOMARK_DELETE(verticesCopied_);
            verticesCopied_ = NULL;
        }
        for(i = 0; i < 1; i++)
        {
            if(verticesMarked_[i])
            {
                GEOMARK_DELETE(verticesMarked_[i]);
                verticesMarked_[i] = NULL;
            }
        }
        if(numAdjVertices_)
        {
            GEOMARK_FREE(numAdjVertices_);
            numAdjVertices_ = NULL;
        }
    }
    void        init(int np, int nf);

    void        meshComplete(void);                                                 /* mesh generation (via setVertex(),setFace())
                         is complete */

    /* for initXZGrid(), importVRS() and importOBJ() no meshComplete()-
    call is required ! */
    /* lower left corner is in origin */
    void        initXZGrid(double widthX, double heightZ, int numXSegments,
                           int numZSegments);

    /* if trId1 and trId2 share an edge, the vertex of trId2 not 
    part of this edge is returned.
    in all other cases -1 is returned ! 

    parameter shared returns indices of trId1 {0,..,2} that were matched
    in the following form:

    0: first and second vertex of trId1 formed the edge
    1: second and third "
    2: first and third  "
  */
    int         getUnsharedVertex(int trId1, int trId2, int *shared);
    int         getUnsharedVertex(int *vIds, int trId2, int *shared);
    int         numberOfSharedVertices(int trId1, int trId2);

    /* returns number of shared vertices and fills array with
    according vertex indices */
    int         getSharedVertices(int trId1, int trId2, int *vIds);                 /* vIds is int[3] */

    /* returns first unshared vertices part of triangle trId1, then trId2 */
    int         getUnsharedVertices(int trId1, int trId2, int *vIds);               /* vIds is int[3] */

    /* checks only if triangle contains doubled vertex indices */
    BOOL        isDegenerated(int trId);

    /* checks if one of triangle edge lengths is below edgeminLength 
    (FRACTION of bounding box)
    and edge length ratios are within llimit<=ratio<=ulimit */
    /* edgeMinLength=this->getBoundingBoxDiameter()*minLengthFraction */
    BOOL        hasDegeneratedShape(int trId, DegenSpec *degenSpec);

    TinyVector  getVertex(int vertexId);
    void        setVertex(int vertexId, const TinyVector &);
    void        setFace(int triangleId, int i1, int i2, int i3,
                        BOOL updateHashTable = TRUE);
    int         vertices(void);
    int         triangles(void);
    int         getTriangleVertex(int triId, int vertNo);
    int         getTriangleVertices(int trId, int *vIds);

    /* tells if vertex lies on border. vertexId must
    be in 2-manifold area (2-manifold vertex)

    borderVertexIds contains vertices in 1-ring of vertexId,
	along the border continues

    numBorderVertices is usually 2. It is larger, if vertexId
	connects non-edge adjacent triangles. However we only
	store max. 2 vertices in borderVertexIds.
  */
    BOOL        isBorderVertex(int vertexId, int *numBorderVertices = NULL,
                               int *borderVertexIds = NULL);

    /* edge related functions:
    edges are not explicitly stored, so the isEdge()-test is slow ! 
  */
    BOOL        isEdge(int vId1, int vId2, double *length);

    /* no check for real edge, just returns distance of points */
    double      edgeLength(int vId1, int vId2);

    /* -1 if not, -2 if face is degenerated,
    >=0 index of unshared vertex in triangle */
    int         isEdgePartOfTriangle(int trId, int vId1, int vId2);

    /* vertex ids of edge (vId1<vId2) opposite to vertex vId
    in triangle trId */
    int         getOppositeEdge(int trId, int vId, int *vId1Edge, int *vId2Edge);

    /* triangles with same vertex ids get same hash values
    (0 <= hash value < number of triangles).
  */
    int         getTriangleHash(int trId);

    int         getNumAdjVertices(int vertexId);
    int         getNumAdjTriangles(int vertexId);
    int         getNumVerticesWithValence(int valence);

    int         getAdjacentTriangles(int vertexId, GrowArray *adjTrianglesList);    /* triangles containig vertex */

    /* adjVertexList can be NULL (otherwise reuse of array) */
    GrowArray   *getAdjacentVertices(int vertexId, GrowArray *adjVertexList);       /* vertices (id's) adjacent to vertex */

    /* adjTriangleList can be NULL (otherwise reuse of array) */
    GrowArray   *getAdjacentTrianglesFromTriangle(int triangleId,
                                                  GrowArray *adjTriangleList,
                                                  int triangleSelection = TRIANGLE_SEL_NO_DOUBLE_TRIDS |
                                                  TRIANGLE_SEL_NO_DEGEN);           /* get indices of all triangles (vertex-)adjacent to one with id triangleId */

    /* WARNING: adjTriangleList must be output of getAdjacentTrianglesFromTriangle() ! */
    GrowArray   *getEdgeAdjacentTriangles(int triangleId,
                                          GrowArray *adjTriangleList,
                                          int triangleSelection = TRIANGLE_SEL_NO_DOUBLE_TRIDS |
                                          TRIANGLE_SEL_NO_DEGEN);                   /* get indices of all triangles (edge-)adjacent to one with id triangleId */

    TinyVector  triangleNormal(int triangleId);                     /* calculate triangle normal */
    int         vertexNormal(int vertexId, TinyVector *normal);     /* equally weighted average of face normals
                                           (degenerated faces for which no
                                           normals can be calculated are
                                           ignored). returns number
                                           of face normals averaged */
    int         vertexNormalWeightedByFaceSizes(int vertexId,
                                                TinyVector *normal,
                                                DegenSpec *spec);   /* as before
											but weighted by face sizes */

    /* calculate normals for whole set, tells degenerated triangles */
    /* function allocates array objects if needed !  */
    int         triangleNormals(GrowArray **normals,
                                GrowBitArray **degeneratedTriangles,
                                DegenSpec *degenSpec = NULL);
    int         triangleNormals(TinyVector *normals,
                                GEOMARK_BitArray *degeneratedTriangles,
                                DegenSpec *degenSpec = NULL);

    /* calculate faces sizes */
    int         faceSizes(GrowArray **sizes);
    double      faceSize(int triangleId);

    /* there are two versions of VRS files, 1 and 2. They differ in
     only one fact: for version 2 a face description is preceeded by
     the number of vertices belonging to that face.
  */
    //void        import_(int fileType, std::istream & file, int version);

    //int         importVRS(std::istream & file, int version = 1);
    //int         importOFF(std::istream & file);
#if 0
    /* precision is number of decimal digits */
    int         exportVRS(std::ostream & file, int version = 1, int precision = 6);

    /* import models in wavefront's obj file format */
    /* works with wcvt2pov-output */
    int         importOBJ(std::istream & file);
    int         exportOBJ(std::ostream & file, int precision = 10); //=6
    int         export2DVecFormat(std::ostream & file,
                                  int vecFormat,                    /* vecFormat e 2DVECFORMAT.. */
                                  GEOMARK_BitArray * markedFaces = NULL);

    int         exportQhull(std::ostream & file);
    int         exportHoppe(std::ostream & file);
#endif
    /* code "borrowed" from SPD library */
    //int importDXF(FILE *file); /* TO BE CONTINUED */
    /* center of mass of model */
    TinyVector  centerOfMass(void);

    /* gives lower left front and upper right back points of
    the triangle sets bounding box. edges of this box
	are parallel to the coordinate axes.    
  */
    void        getBoundingBox(TinyVector *llf, TinyVector *urb,
                               BOOL forceRecalculation = FALSE);
    double      getBoundingBoxDiameter(BOOL forceRecalculation = FALSE);
    double      getLongestEdgeOfBoundingBox(BOOL forceRecalculation = FALSE);

    /* gives largest distance of two vertices in triangle set */
    /* dog-slow, needs refinemebt */
    double      getMaxDistTwoVertices(void);

    /* checks if normals are properly oriented.
    in case of already correct normals, returns 0
  */
    /* this function requires the coldet-library
    (see COLDET_PRESENT in triangleset.cpp)
  */
    int         reorientNormals(BOOL queryOnly = TRUE);

    /* moves center of mass into origin and applies uniform scaling
    so bounding box diameter has length 1 */
    int         unitize(TinyMatrix *transform = NULL,
                        BOOL forceRecalculation = TRUE, BOOL doNotScale = FALSE);

    /* move center of mass to origin */
    int         center(TinyMatrix *tramsform = NULL);

    /* the models center of mass is placed in
    the center of unit cube and the model is uniformly scaled, so
    it's completely inside the cube.  

    unit cube means vertex coordinates in range [-0.5...+0.5]
  */
    int         fitInUnitCube(TinyMatrix *transform, BOOL applyToMesh = FALSE);

    /* the models center of mass is placed in
    the center of a unit sphere and the model is uniformly scaled, so
    the longest vector has length 1

    vertex cooridinates in range [-1..+1]
  */
    int         fitInUnitSphere(TinyMatrix *transform, BOOL applyToMesh = FALSE);

    int         transform(TinyMatrix *transform, BOOL inverse = FALSE);

    //int rotate(double angle,int axis); /* angle in degree */
    void        copySet(TriangleSet *sourceTs);

    /* report degeneracies (no caching, every call causes checking).
    if degeneratedVertices is NULL, only degeneratedFaces are
    calculated.
    BitArrays must be pre-allocated.  
  */
    int         reportDegeneracies(DegenSpec *degenSpec,
                                   GEOMARK_BitArray *degeneratedFaces,
                                   GEOMARK_BitArray *degeneratedVertices,
                                   int *numDegenFaces, int *numDegenVertices);
};

/* for qsort */
int     compareFacesByVertexIndex(const void *e1, const void *e2);
int     compareEdgesFirstVertexThenSecond(const void *e1, const void *e2);
int     compareEdgesSecondVertexThenFirst(const void *e1, const void *e2);
int     compareFacesByComZ(const void *e1, const void *e2);
int     compareAngles(const void *e1, const void *e2);

BOOL    hasDegeneratedShape(TinyVector v1, TinyVector v2, TinyVector v3,
                            DegenSpec *degenSpec);
#endif
#endif
