#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#ifndef _UTIL_H_
#define _UTIL_H_

//#include <math.h> /* for PI */
//#include <assert.h>
#include "OSGWMtinyvector.h"
#include "OSGWMtinymatrix.h"
#include "OSGWMtriangleset.h"
#include "OSGWMarray.h"

/* for backwards compatability, remove as soon as possible */
/* for use with qsort */
typedef struct
{
    int     origIndex;      /* index */
    double  value;          /* value */
    int     gp1;            /* general purpose -> use freely */
    double  gp2;            /* general purpose -> use freely */
} SortEntry;

/* ---------------- */
typedef struct
{
    int     index;
    double  value;
} SortDouble;

typedef struct
{
    TinyVector  center;
    TinyVector  xDir;       /* normalized */
    TinyVector  yDir;       /* normalized */
} TriangleCoordSystem;

typedef struct
{
    TinyVector  center;
    TinyVector  normal;
    double      distToOrigin;
} GEOMARK_Plane;

typedef struct
{
    TinyVector      center;
    TinyVector      xDir;   /* all normalized */
    TinyVector      yDir;
    TinyVector      zDir;
    GEOMARK_Plane   xPlane, yPlane, zPlane;
} RotCoordSystem;

/* vertex/face highlighting */
int         storeSelectedFacesForOBJViewer(GEOMARK_BitArray *selectedFaces,
                                           char *filename);

/* returns a vector orthogonal to given vector */
/* obsolete: use TinyVector::perpendicularNormal() instead */
//int getOrthVector(TinyVector inV,TinyVector *outV);
/* measures enclosed angle between vector a and b, output in radians */
double      vectorAngle(TinyVector a, TinyVector b);

//TinyVector triangleNormal(int triangleId,TriangleSet *mesh); /* calculate triangle normal */
int         triangleNormals(TriangleSet *poly, GrowArray **normals,
                            GrowBitArray **degeneratedTriangles);   /* calculate normals for whole set, tells degenerated triangles */

/* function allocates array objects ! */
double      triangleSurface(TinyVector a, TinyVector b, TinyVector c);

/* rotate vectors a and b which are orthogonal to each other, so b lies on 
  positive z axis and a on positive y axis and return the transformation matrix */
int         rotateOrthVectorsOntoYZ(TinyVector a, TinyVector b,
                                    TinyMatrix *matrix);

/* rotate and translate so line points in direction (0,1,0)   
  and startPoint is in (0,0,0) */
int         rotateVectorOntoY(TinyVector direction, TinyMatrix *roty,
                              TinyMatrix *rotx, double *yAlpha, double *xAlpha);

/* lot from point to line */
int         lotPointLine(TinyVector p, TinyVector lp, TinyVector lrv,
                         TinyVector *result);

/* lot from point to plane (given in parammetric form). Returns
  relative position on plane (skalars) and if lot-point is contained
  inside triangle */
int         lotPointPlaneExplicit(TinyVector pp, TinyVector rv1, TinyVector rv2,
                                  TinyVector p, double *alpha, double *beta,
                                  TinyVector *lotp, BOOL *insideTriangle,
                                  double *distanceFromPlane,
                                  double *distanceFromCentroid);

int         calcPlane(TinyVector p, TinyVector rv1, TinyVector rv2,
                      GEOMARK_Plane *result);
double      distPointPlane(TinyVector p, GEOMARK_Plane *plane);

/* plane normal must be normalized */
int         lotPointPlane(TinyVector point, GEOMARK_Plane *plane,
                          TinyVector *result);
int         distPointPlane(TinyVector point, GEOMARK_Plane *plane,
                           double *distance);

/* factor lambda for line equation which gives point on line
  with distance dist to point P. lambda1 is >= 0 or
  lambda1 < 0 and lambda2 < 0 */
int         pointOnLineWithDistToPoint(TinyVector lp, TinyVector lv,
                                       TinyVector p, double dist,
                                       double *lambda1, double *lambda2);

int         dumpMemBits(void *adr, int numBytes);
int         dumpMemHex(void *adr, int numBytes);

int         compareDouble(const void *elem1, const void *elem2);

/* for backwards compatability, remove as soon as possible */
int         compareSortEntries(const void *elem1, const void *elem2);

/* -------------- */
int         triangleEdgeLengths(int triangleId, TriangleSet *mesh,
                                double *lengths);
int         triangleEdgeLengths(TinyVector a, TinyVector b, TinyVector c,
                                double *lengths);
TinyVector  triangleCenter(int triangleId, TriangleSet *mesh);
TinyVector  triangleCenter(TinyVector v0, TinyVector v1, TinyVector v2);
double      triangleHeight(TinyVector a, TinyVector b, TinyVector c);

TinyVector  triangleNormal(TinyVector a, TinyVector b, TinyVector c);

/* intersection line/plane */
/*
  alpha gives following information:
  alpha<0: rear intersection
  0<=alpha<=1: intersection point between lp and lp+lrv1 
*/
int         intersectionLinePlane(TinyVector lp, TinyVector lrv, TinyVector pp,
                                  TinyVector prv1, TinyVector prv2,
                                  double *alpha, TinyVector *result);

int         intersectionLinePlane(TinyVector lp, TinyVector lrv, TinyVector pp,
                                  TinyVector normal, double *alpha,
                                  TinyVector *result);

/* checks if point is contained in triangle. Everything is in 2D, only
  the first two coordinates of vectors are used (x,y) */
/* does not depend on triangle point order ! */
int         pointInTriangle2D(TinyVector p, TinyVector t1, TinyVector t2,
                              TinyVector t3);

/* the rectangle must have 90 degree angles ! */
int         pointInRectangle2D(TinyVector p, TinyVector t1, TinyVector t2,
                               TinyVector t3, TinyVector t4);

/* params are tetrahedron points */
int         volumeTetrahedronDetBased(TinyVector t1, TinyVector t2,
                                      TinyVector t3, TinyVector t4,
                                      double *volume);

int         volumeTetrahedronExplizit(TinyVector v1, TinyVector v2,
                                      TinyVector v3, TinyVector v4,
                                      double *volume);

/* "boolean test".
  (a,b) are endpoints of a line. if there is only a start
  point and a direction a "usable" endpoint has to be calculated.
  Following workaround: For startpoint and direction calculate
  the two intersections with the bounding box. Then apply the test
  for both endpoints (or only one endpoint if no rear intersection
  is allowed).
*/
int         doesLineIntersectTriangle(TinyVector a, TinyVector b, TinyVector t1,
                                      TinyVector t2, TinyVector t3,
                                      BOOL *intersects);

/* point must have been projected in triangle plane ! */
int         doesProjPointLieInTriangle(TinyVector p, TinyVector a, TinyVector b,
                                       TinyVector c, BOOL *inTriangle,
                                       double *bariCoords);

/* point must have been projected in triangle plane ! */
/* point (should) be outside triangle  ! */
int         calcNearestPointOnTriangleBoundaryForProjPoint(TinyVector p,
                                                           TinyVector a,
                                                           TinyVector b,
                                                           TinyVector c,
                                                           TinyVector normal, TinyVector *nearestPoint,
                                                           double *minDist,
                                                           double *bariCoords);

/* box edges are parallel to coordinate axes ! */
/* if insideBox is TRUE, isp1 contains "nearest" 
  intersection, isp2 second "nearest" rear intersection.
  Use lambda values to determine if its a front/rear-intersection.

  if insideBox is FALSE 
    - and if noFrontIntersections is TRUE, isp1 contains the
	    nearset valid rear intersection, isp2 the second nearest.
		valid means the intersection point lies within box face 
		(or boundary)
    - and if noFrontIntersections is FALSE, isp1 contains the
	    nearset valid front intersection, isp2 the second-nearest
		front intersection.

  IMPORTANT:

  isp1, isp2 contain values only if insideBox or intersectsBox
  is TRUE !

*/
int         intersectionRayBox(TinyVector startPoint, TinyVector direction,
                               TinyVector llfBox, double boxEdgeLength,
                               int *numIntersections, BOOL *insideBox,
                               BOOL *noFrontIntersections, BOOL *intersectsBox,
                               TinyVector *isp1, TinyVector *isp2,
                               double *lambda1, double *lambda2);

/* rotAxis starts in origin and is normalized, angle in radians */
int         rotateVertexAroundAxis(TinyVector *vertex, TinyVector rotAxis,
                                   double angle);

/* vertex and rotAxis-start point already translated into origin */
/* vertex needs to be normalized ! */
int         rotateVertexIntoPlane(TinyVector *vertex, TinyVector rotAxis,
                                  TinyVector planeNormal);

#if 0

/* OBSOLETE */
double      distancePointFromLine(TinyVector p, TinyVector a, TinyVector b);

TinyMatrix  rotateVectorOntoZ(TinyVector a);
TinyMatrix  rotateVectorOntoVector(TinyVector a, TinyVector b);
int         rotateArcOntoArc(TinyVector a, TinyVector b, TinyVector c,
                             TinyVector d, TinyMatrix *m);
int         rotateTriangle(TinyVector p1, TinyVector p2, TinyVector p3,
                           TinyMatrix *m);
void        printMatrix(TinyMatrix &m);

TinyVector  triangleNormal(TinyVector a, TinyVector b, TinyVector c);
TinyVector  triangleCenter(int triangleId, TriangleSet *mesh);

/* calculates lot from c to line with endpoints a,b */
double      triangleHeight(TinyVector a, TinyVector b, TinyVector c);
int         triangleEdgeLengths(int triangleId, TriangleSet *mesh,
                                double *lengths);
int         triangleEdgeLengths(TinyVector a, TinyVector b, TinyVector c,
                                double *lengths);

double      det2(TinyMatrix &m);
double      det3(TinyMatrix &m);

int         calcPlane(TinyVector p, TinyVector rv1, TinyVector rv2,
                      GEOMARK_Plane *result);

/* useful for calcualting distance point/line and point/plane */
int         lotPointLine(TinyVector p, TinyVector lp, TinyVector lrv,
                         TinyVector *result);
int         lotPointPlane(TinyVector point, GEOMARK_Plane *plane,
                          TinyVector *result);

/* dirX: b-lot(c,ab)
   dirY: c-lot(c,ab)
*/
int         triangleCoordSystem(TinyVector a, TinyVector b, TinyVector c,
                                TriangleCoordSystem *coords);
int         world2TriangleCoords(TriangleCoordSystem *coords, TinyVector p,
                                 TinyVector *result);
int         triangle2WorldCoords(TriangleCoordSystem *coords, TinyVector p,
                                 TinyVector *result);

/* rotation of point p around line with endpoints lp1,lp2  */
int         rotCoordSystem(TinyVector lp1, TinyVector lp2, TinyVector p,
                           RotCoordSystem *coords);
int         world2RotCoords(RotCoordSystem *coords, TinyVector p,
                            TinyVector *result);
int         rot2WorldCoords(RotCoordSystem *coords, TinyVector p,
                            TinyVector *result);

int         adjustVector(TinyVector lp1, TinyVector lp2, TinyVector p,
                         TinyVector z, double length, TinyVector *result);

void        dumpModelLengthsAndAngles(TriangleSet *mesh);

/* inverting 2x2 matrix */
int         invert2(TinyMatrix *m, TinyMatrix *result);
#endif
#endif
#endif
