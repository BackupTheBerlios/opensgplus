#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)

//#include <fstream>
//#include <iostream>
#include "OSGWMmylimits.h"
#include "OSGWMutil.h"

/* tolerances may depend on scaling of model ! */
static const double ZERO_TOL = 1e-06;
static const double ZERO_DENOM_TOL = 1e-10;
static const double ZERO_VOL_TOL = 1e-05;
static const double ZERO_TRIANGLE_SURFACE_TOL = 1e-06;
static const double ZERO_DIFF_TO_ONE_TOL = 1e-06;

int                 global_debug = 0;

/* global var, activates some debug output */

/* */

int compareDouble(const void *elem1, const void *elem2)
{
    SortDouble  e1, e2;

    e1 = *(SortDouble *) elem1;
    e2 = *(SortDouble *) elem2;

    if(e1.value < e2.value)
        return -1;
    else if(e1.value == e2.value)
        return 0;
    else
        return 1;
}

/* for backwards compatability, remove as soon as possible */
int compareSortEntries(const void *elem1, const void *elem2)
{
    SortEntry   e1, e2;

    e1 = *(SortEntry *) elem1;
    e2 = *(SortEntry *) elem2;

    if(e1.value < e2.value)
        return -1;
    else if(e1.value == e2.value)
        return 0;
    else
        return 1;
}

/* -------------- */
double vectorAngle(TinyVector a, TinyVector b)
{
    double  val, quot;

    //val=fabs(dotProduct(a,b));
    val = dotProduct(a, b);
    quot = tv_abs(a) * tv_abs(b);

    return safe_acos(val / quot);
}

/* */
int storeSelectedFacesForOBJViewer(GEOMARK_BitArray *selectedFaces,
                                   char *filename)
{
    int i, n;
#if 0
    std::ofstream outFile(filename);

    n = selectedFaces->getMaxIndex() + 1;
    outFile << "1 0 \n";
    for(i = 0; i < n; i++)
    {
        if(selectedFaces->getBit(i))
            outFile << "1\n";
        else
            outFile << "0\n";
    }

    outFile.close();
#endif
    return 0;
}

/* */
double triangleSurface(TinyVector a, TinyVector b, TinyVector c)
{
    double  d, e;

    return tv_abs((b - a) * (c - a)) / 2.;
}

/* */
int rotateOrthVectorsOntoYZ(TinyVector a, TinyVector b, TinyMatrix *matrix)
{
    double      d1, d2, d3, zt, xt, yt;
    double      alpha;
    TinyMatrix  rotx, roty, rotz;
    TinyVector  a2, b2, a3, b3, a4, b4;

    /* first step: rotate b around y into (positive) yz plane */
    zt = b[2];
    xt = b[0];

    d1 = sqrt(zt * zt + xt * xt);
    if(fabs(d1) <= ZERO_TOL)
    {
        roty.identity();
    }
    else
    {
        alpha = asin(xt / d1);

        if(zt < 0)
        {
            alpha = -(PI - fabs(alpha));
            if(xt < 0)
                alpha = -alpha;
        }
        else
            alpha = -alpha;

        roty.rot(DEGREE(alpha), 1);
    }       /* < ZERO_TOL */

    a2 = roty * a;
    b2 = roty * b;

    /* second step: rotate b2 around x onto positive z axis */
    yt = b2[1];
    zt = b2[2];

    d2 = sqrt(yt * yt + zt * zt);
    if(fabs(d2) <= ZERO_TOL)
    {
        rotx.identity();
    }
    else
    {
        alpha = asin(yt / d2);

        if(zt < 0)
        {
            alpha = PI - fabs(alpha);
            if(yt < 0)
                alpha = -alpha;
        }   //else alpha=-alpha;

        rotx.rot(DEGREE(alpha), 0);
    }       /* < ZERO_TOL */

    a3 = rotx * a2;
    b3 = rotx * b2;

    /* third step: rotate a3 around z into (positive) yz plane */
    /* if a and b were orthogonal, a4 lies on positive y axis */
    xt = a3[0];
    yt = a3[1];

    d3 = sqrt(xt * xt + yt * yt);
    if(fabs(d3) <= ZERO_TOL)
    {
        rotz.identity();
    }
    else
    {
        alpha = asin(xt / d3);

        if(yt < 0)
        {
            alpha = PI - fabs(alpha);
            if(xt < 0)
                alpha = -alpha;
        }   //else alpha=-alpha;

        rotz.rot(DEGREE(alpha), 2);
    }       /* < ZERO_TOL */

    *matrix = rotz * rotx * roty;

    return 0;
}

/* */
int rotateVectorOntoY(TinyVector direction, TinyMatrix *ry, TinyMatrix *rx,
                      double *yAlpha, double *xAlpha)
{
    TinyVector  a, a2;
    TinyMatrix  roty, rotx;
    double      xt, yt, zt, d1, d2, alpha, alphay, alphax;

    /* first step: rotate direction around y into (positive) yz plane */
    a = direction;

    zt = a[2];
    xt = a[0];

    d1 = sqrt(zt * zt + xt * xt);
    if(fabs(d1) <= ZERO_TOL)
    {
        roty.identity();
        alphay = 0;
    }
    else
    {
        alpha = asin(xt / d1);
        if(zt < 0)
        {
            alpha = -(PI - fabs(alpha));
            if(xt < 0)
                alpha = -alpha;
        }
        else
            alpha = -alpha;

        alphay = DEGREE(alpha);

        roty.rot(alphay, 1);
    }       /* < ZERO_TOL */

    a2 = roty * a;

    /* second step: rotate a2 around x onto y axis */
    yt = a2[1];
    zt = a2[2];

    d2 = sqrt(yt * yt + zt * zt);
    if(fabs(d2) <= ZERO_TOL)
    {
        rotx.identity();
        alphax = 0;
    }
    else
    {
        alpha = asin(zt / d2);

        if(yt < 0)
        {
            alpha = -(PI - fabs(alpha));
            if(zt < 0)
                alpha = -alpha;
        }
        else
            alpha = -alpha;

        alphax = DEGREE(alpha);

        rotx.rot(alphax, 0);
    }       /* < ZERO_TOL */

    TinyVector  test;
    test = rotx * roty * direction;

    *ry = roty;
    *rx = rotx;
    *yAlpha = alphay;
    *xAlpha = alphax;

    return 0;
}

/* 
   Find nadir of plumb from a point to a line. 
   Find vector from starting point of the line to nadir.

   lp:  line start point.
   lrv: line direction.
   p: point NOT on the line.
*/
int lotPointLine(TinyVector p, TinyVector lp, TinyVector lrv, TinyVector *result)
{
    double  alpha;
    double  z, n;

    // Norm of vector lrv
    n = lrv[0] * lrv[0] + lrv[1] * lrv[1] + lrv[2] * lrv[2];
    if(n <= ZERO_TOL_EDGE_LENGTH)
        return -1;

    // Dot product of vector lrv and vector (p-lp)
    z = lrv[0] * (p[0] - lp[0]) + lrv[1] * (p[1] - lp[1]) + lrv[2] * (p[2] - lp[2]);

    // Length of projection of vector (p-lp) onto vector lrv.
    alpha = z / n;

    // vector pointing to nadir.
    (*result)[0] = lp[0] + alpha * lrv[0];
    (*result)[1] = lp[1] + alpha * lrv[1];
    (*result)[2] = lp[2] + alpha * lrv[2];

    return 0;
}

/* faster and more stable method ? */
int lotPointPlaneExplicit(TinyVector pp, TinyVector rv1, TinyVector rv2,
                          TinyVector p, double *alpha, double *beta,
                          TinyVector *lotp, BOOL *insideTriangle,
                          double *distanceFromPlane,
                          double *distanceFromCentroid)
{
    int         i, rv;
    TinyVector  normal;
    TinyMatrix  matrix, imatrix;
    TinyVector  rv1n, rv2n;
    TinyVector  result;
    TinyVector  v1, v2, centroid;

    rv1n = rv1.normalized();
    rv2n = rv2.normalized();

    normal = rv1n * rv2n;
    normal.normalize();

    for(i = 0; i < 3; i++)
    {
        matrix[i][0] = rv1[i];
        matrix[i][1] = rv2[i];
        matrix[i][2] = -normal[i];
    }

    rv = matrix.inverse(imatrix);

    if(rv != 0)
        return -1;

    imatrix.transpose();
    result = imatrix * (p - pp);

    *lotp = pp + rv1 * result[0] + rv2 * result[1];
    *alpha = result[0];
    *beta = result[1];
    if((result[0] >= 0) && (result[1] >= 0) && (result[0] + result[1] <= 1))
        *insideTriangle = TRUE;
    else
        *insideTriangle = FALSE;
    *distanceFromPlane = tv_abs(p -*lotp);
    v1 = pp + rv1;
    v2 = pp + rv2;
    centroid = (pp + v1 + v2) / 3.;
    *distanceFromCentroid = tv_abs(p - centroid);

    //  result.dump();
    return 0;
}

/* */
int calcPlane(TinyVector p, TinyVector rv1, TinyVector rv2,
              GEOMARK_Plane *result)
{
    TinyVector  normal;
    double      d;

    /* calculate Hess'sche Normalenform */
    rv1.normalize();
    rv2.normalize();

    normal = rv1 * rv2;
    normal.normalize();
    d = dotProduct(p, normal);

    result->center = p;
    result->normal = normal;
    result->distToOrigin = d;

    return 0;
}

/* */
double distPointPlane(TinyVector p, GEOMARK_Plane *plane)
{
    return dotProduct(plane->normal, p) - plane->distToOrigin;
}

/* */
int lotPointPlane(TinyVector p, GEOMARK_Plane *plane, TinyVector *result)
{
    TinyVector  lot, n;
    double      d;

    n = plane->normal;
    d = plane->distToOrigin;

    //lot=p+n*(d-(dotProduct(n,p))/dotProduct(n,n));
    lot = p + n * (d - dotProduct(n, p));       /* n is normalized */

    *result = lot;

    return 0;
}

/* */
int distPointPlane(TinyVector point, GEOMARK_Plane *plane, double *distance)
{
    *distance = dotProduct(plane->normal, point) - plane->distToOrigin;
    return 0;
}

/* */
int pointOnLineWithDistToPoint(TinyVector lp, TinyVector lv, TinyVector p,
                               double dist, double *lambda1, double *lambda2)
{
    TinyVector  h;
    double      a, b, c, d, l1, l2;

    h = p - lp;
    a = lv[0] * lv[0] + lv[1] * lv[1] + lv[2] * lv[2];
    b = -2. * (h[0] * lv[0] + h[1] * lv[1] + h[2] * lv[2]);
    c = h[0] * h[0] + h[1] * h[1] + h[2] * h[2] - (dist * dist);

    d = b * b - 4 * a * c;

    if(fabs(a) < 1e-09)
        return -1;                              /* no point with this distance */
    if(d < 0)
        return -1;                              /* " */

    d = sqrt(d);
    l1 = (-b + d) / (2. * a);
    l2 = (-b - d) / (2. * a);

    if(l1 > 0)
    {
        *lambda1 = l1;
        *lambda2 = l2;
    }
    else
    {
        *lambda1 = l2;
        *lambda2 = l1;
    }

    return 0;
}

/* */
int dumpMemBits(void *adr, int numBytes)
{
    int             i, j, val;
    unsigned char   b;

    printf("MEM-DUMP-BITS:\n");
    printf(">");
    for(i = 0; i < numBytes; i++)
    {
        b = ((unsigned char *) adr)[i];
        for(j = 0; j < 8; j++)
        {
            val = ((b & (0x80 >> j)) > 0);
            if(val == 0)
                printf("0");
            else
                printf("1");
        }
    }

    printf("<\n");

    return 0;
}

static char hexStr[] = "0123456789abcdef";

/* */

int dumpMemHex(void *adr, int numBytes)
{
    int             i, j, val;
    unsigned char   b;
    int             hd;

    printf("MEM-DUMP-BITS:\n");
    printf(">");
    for(i = 0; i < numBytes; i++)
    {
        b = ((unsigned char *) adr)[i];
        hd = b >> 4;
        printf("%c", hexStr[hd]);
        hd = b & 0x0f;
        printf("%c", hexStr[hd]);
    }

    printf("<\n");

    return 0;
}

/* */
int triangleEdgeLengths(int triangleId, TriangleSet *mesh, double *lengths)
{
    int         i;
    TinyVector  v[3];

    for(i = 0; i < 3; i++)
    {
        v[i] = mesh->getVertex(mesh->getTriangleVertex(triangleId, i));
    }

    lengths[0] = tv_abs(v[1] - v[0]);
    lengths[1] = tv_abs(v[2] - v[1]);
    lengths[2] = tv_abs(v[0] - v[2]);

    return 0;
}

/* */
int triangleEdgeLengths(TinyVector a, TinyVector b, TinyVector c,
                        double *lengths)
{
    lengths[0] = tv_abs(b - a);
    lengths[1] = tv_abs(c - b);
    lengths[2] = tv_abs(a - c);

    return 0;
}

/* */
TinyVector triangleCenter(int triangleId, TriangleSet *mesh)
{
    TinyVector  center, v0, v1, v2;

    v0 = mesh->getVertex(mesh->getTriangleVertex(triangleId, 0));
    v1 = mesh->getVertex(mesh->getTriangleVertex(triangleId, 1));
    v2 = mesh->getVertex(mesh->getTriangleVertex(triangleId, 2));

    center = (v0 + v1 + v2) / 3.;
    return center;
}

/* */
TinyVector triangleCenter(TinyVector v0, TinyVector v1, TinyVector v2)
{
    TinyVector  center = (v0 + v1 + v2) / 3.;
    return center;
}

/* */
double triangleHeight(TinyVector a, TinyVector b, TinyVector c)
{
    TinyVector  isp;
    TinyVector  isv;
    double      height;

    lotPointLine(c, a, b - a, &isp);

    //std::cout << "isp=" << isp << "\n";
    isv = c - isp;
    height = tv_abs(isv);

    return height;
}

/* */
TinyVector triangleNormal(TinyVector a, TinyVector b, TinyVector c)
{
    TinyVector  normal;
    TinyVector  h1, h2;

    /* BUGFIX */
    h1 = (b - a);
    h1.normalize();
    h2 = (c - a);
    h2.normalize();

    //normal=(b-a)*(c-a);
    normal = h1 * h2;

    normal.normalize();
    return normal;
}

/*
  Method used:

  plane (in Hesssche Normalenform): E: n*x-d=0
  line: l: p+s*lrv

  Insert line in plane equation:

  n1*(p1+s*lrv1)+n2*(p2+s*lrv2)+n3*(p3+s*lrv3)-d=0

  <=>

  s = - (n1*p1 + n2*p2 + n3*p3 + d) / (n1*lrv1 + n2*lrv2+ n3*lrv3)
*/
int intersectionLinePlane(TinyVector lp, TinyVector lrv, TinyVector pp,
                          TinyVector prv1, TinyVector prv2, double *alpha,
                          TinyVector *result)
{
    GEOMARK_Plane   plane;
    double          beta, gamma;
    double          denom;
    TinyVector      isecP;
    int             i, rv;
    BOOL            inside;
    int             debug = 0;
    double          dist1, dist2, distRv1, distRv2;
    TinyVector      a, b, c;

    debug = global_debug;

    if(debug)
    {
        //std::cout << "--normal=" << lp * 0.5 << "\n";
    }

    /* first calculate hess'sche normalenform */
    calcPlane(pp, prv1, prv2, &plane);

    if(debug)
    {
        /*std::cout <<
            "a,b,c=" <<
            pp <<
            "," <<
            pp +
            prv1 <<
            "," <<
            pp +
            prv2 <<
            "\n";
    */}

    if(debug)
    {
/*        std::cout <<
            "HNF: (n,d)=" <<
            plane.normal <<
            "," <<
            plane.distToOrigin <<
            "\n";
*/    }

    denom = plane.normal[0] *
        lrv[0] +
        plane.normal[1] *
        lrv[1] +
        plane.normal[2] *
        lrv[2];

    if(debug)
    {
//        std::cout << "denom=" << denom << "\n";
    }

    if((denom == 0) || (fabs(denom) < 0.0000001))
    {
 /*       if(debug)
            printf("...denom value !\n");
 */       return -1;
    }

    *alpha =
        (
            plane.distToOrigin -
            plane.normal[0] *
            lp[0] -
            plane.normal[1] *
            lp[1] -
            plane.normal[2] *
            lp[2]
        ) /
        denom;

    if(debug)
    {
 //       std::cout << "alpha=" << alpha << "\n";
    }

    //if (((alpha > 1) && (mustBeMiddleIntersection)) ||
    //    ((alpha < 0) && (!rearIntersectionAllowed)))
    //{
    //  if (debug)
    //    printf("...alpha value !\n");
    //  return -1;
    //}
    isecP = lp + lrv * (*alpha);

    if(debug)
    {
 //       std::cout << "intersection point=" << isecP << "\n";
    }

    /* calculate plane relative position of intersection */
    if(debug)
    {
  //      std::cout << "prv1=" << prv1 << "\n";
  //      std::cout << "prv2=" << prv2 << "\n";
    }

    *result = isecP;

    return 0;
}

/* */
int intersectionLinePlane(TinyVector lp, TinyVector lrv, TinyVector pp,
                          TinyVector normal, double *alpha, TinyVector *result)
{
    GEOMARK_Plane   plane;
    double          denom;

    plane.center = pp;
    plane.normal = normal;
    plane.distToOrigin = dotProduct(pp, normal);

    denom = plane.normal[0] *
        lrv[0] +
        plane.normal[1] *
        lrv[1] +
        plane.normal[2] *
        lrv[2];

    if(fabs(denom) < ZERO_DENOM_TOL)
        return -1;

    *alpha =
        (
            plane.distToOrigin -
            plane.normal[0] *
            lp[0] -
            plane.normal[1] *
            lp[1] -
            plane.normal[2] *
            lp[2]
        ) /
        denom;

    *result = lp + lrv * (*alpha);

    return 0;
}

/* this stuff is 2D, only first two vector components are used ! */
int pointInTriangle2D(TinyVector p, TinyVector t1, TinyVector t2, TinyVector t3)
{
    double  b0, b1, b2, b3;

    b0 = (t2[0] - t1[0]) * (t3[1] - t1[1]) - (t3[0] - t1[0]) * (t2[1] - t1[1]);

    if(fabs(b0) <= ZERO_TOL)
        return -1;

    b1 = ((t2[0] - p[0]) * (t3[1] - p[1]) - (t3[0] - p[0]) * (t2[1] - p[1])) / b0;
    b2 = ((t3[0] - p[0]) * (t1[1] - p[1]) - (t1[0] - p[0]) * (t3[1] - p[1])) / b0;
    b3 = ((t1[0] - p[0]) * (t2[1] - p[1]) - (t2[0] - p[0]) * (t1[1] - p[1])) / b0;

    if((b1 < 0) || (b2 < 0) || (b3 < 0))
        return 0;
    else
        return 1;
}

/* */
int pointInRectangle2D(TinyVector p, TinyVector t1, TinyVector t2,
                       TinyVector t3, TinyVector t4)
{
    int rv;

    rv = pointInTriangle2D(p, t1, t2, t3);
    if((rv == -1) || (rv == 1))
        return rv;
    rv = pointInTriangle2D(p, t2, t3, t4);
    return rv;
}

/* */
int volumeTetrahedronDetBased(TinyVector t1, TinyVector t2, TinyVector t3,
                              TinyVector t4, double *volume)
{
    double      vol;

    TinyMatrix  m(4, 4, t1[0], t1[1], t1[2], 1, t2[0], t2[1], t2[2], 1, t3[0],
                  t3[1], t3[2], 1, t4[0], t4[1], t4[2], 1);

    m.det4(&vol);
    vol /= 6.;

    *volume = vol;
    return 0;
}

/* */
int volumeTetrahedronExplizit(TinyVector v1, TinyVector v2, TinyVector v3,
                              TinyVector v4, double *volume)
{
    double          G, h;
    GEOMARK_Plane   plane;
    TinyVector      rv1, rv2;

    rv1 = v2 - v1;
    rv2 = v3 - v1;

    /* calc pyramid height */
    calcPlane(v1, rv1, rv2, &plane);
    distPointPlane(v4, &plane, &h);
    h = fabs(h);

    /* calc area of pyramid base triangle */
    G = tv_abs(rv1 * rv2) * 0.5;

    *volume = G * h * (1. / 3.);

    return 0;
}

/* */
int doesLineIntersectTriangle(TinyVector a, TinyVector b, TinyVector t1,
                              TinyVector t2, TinyVector t3, BOOL *intersects)
{
    double  vol1, vol2, vol3;
    double  tvol1, tvol2, tvol3;
    int     numZeroVolumes;
    int     undefined[3];

    volumeTetrahedronDetBased(a, t2, t3, b, &vol1);
    volumeTetrahedronDetBased(a, t1, t2, b, &vol2);
    volumeTetrahedronDetBased(a, t3, t1, b, &vol3);

    //volumeTetrahedronIntersectionBased(a,t2,t3,b,&tvol1);
    //volumeTetrahedronIntersectionBased(a,t1,t2,b,&tvol2);
    //volumeTetrahedronIntersectionBased(a,t3,t1,b,&tvol3);
    //volumeTetrahedronDetBased(t1,t2,t3,a,&tvol1);
    //volumeTetrahedronDetBased(t1,t2,t3,b,&tvol2);
    /* there are several cases to distinguish:

    all volumes > 0 or all volumes < 0
	  -> intersection point lies inside the triangle
    one volume=0 others same sign (> 0 or < 0)
	  -> intersection-point lies on edge
	three volumes=0
	  -> line "intersects triangle from the side",
	     line lies in triangle plane
	two volumes=0
	  -> should not happen ! 
  */
    numZeroVolumes = 0;

    /* better use ZERO_TOL here ? */
    if(vol1 == 0)
        numZeroVolumes++;
    if(vol2 == 0)
        numZeroVolumes++;
    if(vol3 == 0)
        numZeroVolumes++;

    *intersects = FALSE;

#if 0
    if(numZeroVolumes >= 2)
        return -1;

    //printf("vol1=%f,vol2=%f,vol3=%f\n",vol1,vol2,vol3);
    if(((vol1 <= 0) && (vol2 <= 0) && (vol3 <= 0)) ||
       ((vol1 >= 0) && (vol2 >= 0) && (vol3 >= 0)))
    {
        *intersects = TRUE;
        return 0;
    }
    else
    {
        return -1;
    }

#else
    /* numerically more robust solution */
    if(numZeroVolumes >= 2)
        return -1;

    undefined[0] = 0;
    undefined[1] = 0;
    undefined[2] = 0;
    if(fabs(vol1) < ZERO_VOL_TOL)
        undefined[0] = 1;
    if(fabs(vol2) < ZERO_VOL_TOL)
        undefined[1] = 1;
    if(fabs(vol3) < ZERO_VOL_TOL)
        undefined[2] = 1;

    //printf("vol1=%f,vol2=%f,vol3=%f\n",vol1,vol2,vol3);
    if((
           ((vol1 <= 0) || (undefined[0])) &&
       ((vol2 <= 0) || (undefined[1])) &&
       ((vol3 <= 0) || (undefined[2]))
   ) ||
       (
           ((vol1 >= 0) || (undefined[0])) &&
       ((vol2 >= 0) || (undefined[1])) &&
       ((vol3 >= 0) || (undefined[2]))
   ))
    {
        *intersects = TRUE;
        return 0;
    }
    else
    {
        return -1;
    }
#endif
}

/* */
int doesProjPointLieInTriangle(TinyVector p, TinyVector a, TinyVector b,
                               TinyVector c, BOOL *inTriangle,
                               double *bariCoords)
{
    double  s, t, u, surfABC;

    surfABC = triangleSurface(a, b, c);
    if(surfABC < ZERO_TRIANGLE_SURFACE_TOL)
        return -1;

    s = triangleSurface(b, c, p) / surfABC;
    t = triangleSurface(c, a, p) / surfABC;
    u = triangleSurface(a, b, p) / surfABC;

    *inTriangle =
        (
            (
                (s >= 0) &&
                (s <= 1) &&
                (t >= 0) &&
                (t <= 1) &&
                (u >= 0) &&
                (u <= 1)
            ) &&
            (fabs(s + t + u - 1) < ZERO_DIFF_TO_ONE_TOL)
        );
    bariCoords[0] = s;
    bariCoords[1] = t;
    bariCoords[2] = u;

    return 0;
}

/* */
int calcNearestPointOnTriangleBoundaryForProjPoint(TinyVector p, TinyVector a,
                                                   TinyVector b, TinyVector c,
                                                   TinyVector normal,
                                                   TinyVector *nearestPoint,
                                                   double *minDist,
                                                   double *bariCoords)
{
    GEOMARK_Plane   planes[3];
    double          dist[3];
    double          distVertices[3];
    TinyVector      lot[3];
    int             i;
    BOOL            inTriangle;
    int             minIndex, minIndex2;
    double          bCoords[3], minBCoords[3];

    /* calc planes for all three edges of triangle */
    calcPlane(a, b - a, normal, &planes[0]);
    calcPlane(b, c - b, normal, &planes[1]);
    calcPlane(c, a - c, normal, &planes[2]);

    distVertices[0] = tv_abs(a - p);
    distVertices[1] = tv_abs(b - p);
    distVertices[2] = tv_abs(c - p);

    minIndex = -1;
    minIndex2 = -1;
    minBCoords[0] = 0;
    minBCoords[1] = 0;
    minBCoords[2] = 0;
    for(i = 0; i < 3; i++)
    {
        dist[i] = fabs(distPointPlane(p, &planes[i]));
        lotPointPlane(p, &planes[i], &lot[i]);

        /* check if one of the lots lies on triangle edge */
        /* simpler way ? */
        doesProjPointLieInTriangle(a, b, c, lot[i], &inTriangle, bCoords);
        if((inTriangle) && ((minIndex < 0) || (dist[i] < dist[minIndex])))
        {
            minIndex = i;
            minBCoords[0] = bCoords[0];
            minBCoords[1] = bCoords[1];
            minBCoords[2] = bCoords[2];
        }

        if((minIndex2 < 0) || (distVertices[i] < distVertices[minIndex2]))
            minIndex2 = i;
    }

    if(minIndex != -1)
    {
        *nearestPoint = lot[minIndex];
        *minDist = dist[minIndex];
        bariCoords[0] = minBCoords[0];
        bariCoords[1] = minBCoords[1];
        bariCoords[2] = minBCoords[2];
    }
    else
    {
        /* simply choose the nearest triangle point */
        *minDist = distVertices[minIndex2];

        if(minIndex2 == 0)
        {
            *nearestPoint = a;
            bariCoords[0] = 1;
            bariCoords[1] = 0;
            bariCoords[2] = 0;
        }
        else if(minIndex2 == 1)
        {
            *nearestPoint = b;
            bariCoords[0] = 0;
            bariCoords[1] = 1;
            bariCoords[2] = 0;
        }
        else
        {
            *nearestPoint = c;
            bariCoords[0] = 0;
            bariCoords[1] = 0;
            bariCoords[2] = 1;
        }
    }

    return 0;
}

/* */
int intersectionRayBox(TinyVector startPoint, TinyVector direction,
                       TinyVector llfBox, double boxEdgeLength,
                       int *numIntersections, BOOL *insideBox,
                       BOOL *noFrontIntersections, BOOL *intersectsBox,
                       TinyVector *isp1, TinyVector *isp2, double *lambda1,
                       double *lambda2)
{
    BOOL        intersection[6];
    double      lambda[6];
    int         lambdaCase[6];
    int         i, j, numIs, numValidIs;
    TinyVector  isp[6];
    int         validIs[2];
    double      absDirection[3];
    double      h;
    int         h2;
    BOOL        posValues, negValues;
    int         posIndex, negIndex;
    double      l0, l1;

    for(i = 0; i < 6; i++)
        intersection[i] = FALSE;
    absDirection[0] = fabs(direction[0]);
    absDirection[1] = fabs(direction[1]);
    absDirection[2] = fabs(direction[2]);

    numIs = 0;

    if(absDirection[0] > ZERO_TOL)
    {
        lambda[numIs] = (llfBox[0] - startPoint[0]) / direction[0];
        lambdaCase[numIs] = 0;
        intersection[0] = TRUE;
        numIs++;
    }
    else
        intersection[0] = FALSE;
    if(absDirection[0] > ZERO_TOL)
    {
        lambda[numIs] = ((llfBox[0] + boxEdgeLength) - startPoint[0]) / direction[0];
        lambdaCase[numIs] = 1;
        intersection[1] = TRUE;
        numIs++;
    }
    else
        intersection[1] = FALSE;

    if(absDirection[1] > ZERO_TOL)
    {
        lambda[numIs] = (llfBox[1] - startPoint[1]) / direction[1];
        lambdaCase[numIs] = 2;
        intersection[2] = TRUE;
        numIs++;
    }
    else
        intersection[2] = FALSE;
    if(absDirection[1] > ZERO_TOL)
    {
        lambda[numIs] = ((llfBox[1] + boxEdgeLength) - startPoint[1]) / direction[1];
        lambdaCase[numIs] = 3;
        intersection[3] = TRUE;
        numIs++;
    }
    else
        intersection[3] = FALSE;

    if(absDirection[2] > ZERO_TOL)
    {
        lambda[numIs] = (llfBox[2] - startPoint[2]) / direction[2];
        lambdaCase[numIs] = 4;
        intersection[4] = TRUE;
        numIs++;
    }
    else
        intersection[4] = FALSE;
    if(absDirection[2] > ZERO_TOL)
    {
        lambda[numIs] = ((llfBox[2] + boxEdgeLength) - startPoint[2]) / direction[2];
        lambdaCase[numIs] = 5;
        intersection[5] = TRUE;
        numIs++;
    }
    else
        intersection[5] = FALSE;

    if(numIs > 1)
    {
        /* sort lambdas in ascending order  */
        for(i = 0; i < numIs - 1; i++)
        {
            for(j = i + 1; j < numIs; j++)
            {
                if(fabs(lambda[i]) > fabs(lambda[j]))
                {
                    h = lambda[i];
                    lambda[i] = lambda[j];
                    lambda[j] = h;
                    h2 = lambdaCase[i];
                    lambdaCase[i] = lambdaCase[j];
                    lambdaCase[j] = h2;
                }
            }
        }

        /* get intersection points */
        for(i = 0; i < numIs; i++)
        {
            isp[i] = startPoint + direction * lambda[i];
        }

        i = 0;
        numValidIs = 0;
        while((i < numIs) && (numValidIs < 2))
        {
            if((lambdaCase[i] == 0) || (lambdaCase[i] == 1))
            {
                if((
                       (isp[i][1] >= llfBox[1]) &&
                   (isp[i][1] <= llfBox[1] + boxEdgeLength)
               ) &&
                   (
                       (isp[i][2] >= llfBox[2]) &&
                   (isp[i][2] <= llfBox[2] + boxEdgeLength)
               ))
                {
                    validIs[numValidIs] = i;
                    numValidIs++;
                }
            }
            else if((lambdaCase[i] == 2) || (lambdaCase[i] == 3))
            {
                if((
                       (isp[i][0] >= llfBox[0]) &&
                   (isp[i][0] <= llfBox[0] + boxEdgeLength)
               ) &&
                   (
                       (isp[i][2] >= llfBox[2]) &&
                   (isp[i][2] <= llfBox[2] + boxEdgeLength)
               ))
                {
                    validIs[numValidIs] = i;
                    numValidIs++;
                }
            }
            else if((lambdaCase[i] == 4) || (lambdaCase[i] == 5))
            {
                if((
                       (isp[i][0] >= llfBox[0]) &&
                   (isp[i][0] <= llfBox[0] + boxEdgeLength)
               ) &&
                   (
                       (isp[i][1] >= llfBox[1]) &&
                   (isp[i][1] <= llfBox[1] + boxEdgeLength)
               ))
                {
                    validIs[numValidIs] = i;
                    numValidIs++;
                }
            }

            i++;
        }

        *numIntersections = numValidIs;

        if(numValidIs <= 1)
        {
            *insideBox = FALSE;
            *intersectsBox = FALSE;
            *noFrontIntersections = FALSE;
        }
        else
        {
            /* 
	    if lambda values of valid intersection have different signs,
		choose smallest (abs) positive and negative value.
		if all valid intersections have equal sign, simply choose
		two smallest (abs).
	  */
            posValues = FALSE;
            negValues = FALSE;
            i = 0;
            while((i < numValidIs) && ((!posValues) || (!negValues)))
            {
                if((!negValues) && (lambda[validIs[i]] < 0))
                {
                    negValues = TRUE;
                    negIndex = validIs[i];
                }
                else if(!posValues)
                {
                    posValues = TRUE;
                    posIndex = validIs[i];
                }

                i++;
            }

            if((posValues) && (negValues))
            {
                l0 = lambda[negIndex];
                l1 = lambda[posIndex];
                lambda[0] = l0;
                lambda[1] = l1;
            }
            else
            {
                /* simply choose lambda[0] and lambda[1] */
                negIndex = validIs[0];
                posIndex = validIs[1];
                l0 = lambda[negIndex];
                l1 = lambda[posIndex];
                lambda[0] = l0;
                lambda[1] = l1;
            }

            *insideBox =
                (
                    ((lambda[0] <= 0) && (lambda[1] >= 0)) ||
                    ((lambda[0] >= 0) && (lambda[1] <= 0))
                );
            *noFrontIntersections = ((lambda[0] < 0) && (lambda[1] < 0));

            *intersectsBox = (numValidIs >= 2); /* ==2 not sufficient, since ray
									     may pass through an edge */

            *isp1 = isp[negIndex];
            *isp2 = isp[posIndex];
            *lambda1 = lambda[0];
            *lambda2 = lambda[1];
        }
    }                               /* if numIs>1 */
    else
        *numIntersections = numIs;

    if(numIs <= 1)
        return -1;
    else
        return 0;
}

/* */
int rotateVertexAroundAxis(TinyVector *vertex, TinyVector rotAxis, double angle)
{
    double      co, si;
    TinyMatrix  rot;
    double      a, b, c;

    a = rotAxis[0];
    b = rotAxis[1];
    c = rotAxis[2];

    co = cos(angle);
    si = sin(angle);
    rot[0][0] = a * a * (1 - co) + co;
    rot[0][1] = a * b * (1 - co) - c * si;
    rot[0][2] = a * c * (1 - co) + b * si;

    rot[1][0] = a * b * (1 - co) + c * si;
    rot[1][1] = b * b * (1 - co) + co;
    rot[1][2] = b * c * (1 - co) - a * si;

    rot[2][0] = a * c * (1 - co) - b * si;
    rot[2][1] = b * c * (1 - co) + a * si;
    rot[2][2] = c * c * (1 - co) + co;

    *vertex = rot * (*vertex);

    return 0;
}

/* */
int rotateVertexIntoPlane(TinyVector *vertex, TinyVector rotAxis,
                          TinyVector planeNormal)
{
    double      d, h;
    TinyMatrix  rotX, rotY, rotZ, transf, itransf;
    TinyVector  v, v2, r2, n2, testV;
    double      dZeroTol = 1e-08;   /* HARDCODED */
    double      hZeroTol = 1e-08;   /* HARDCODED */
    double      diffZeroTol = 1e-06;            /* HARDCODED */
    double      alpha2, alpha, angle;
    double      cosa, sina;

    v = *vertex;

    rotateOrthVectorsOntoYZ(planeNormal, rotAxis, &transf);
    v2 = transf * v;
    r2 = transf * rotAxis;
    n2 = transf * planeNormal;
#if 0
    printf(">>--\n");
    printf("v2=");
    v2.dump();
    printf("r2=");
    r2.dump();
    printf("n2=");
    n2.dump();
    printf("length=%f,%f,%f\n", tv_abs(v2), tv_abs(r2), tv_abs(n2));
    printf("%f\n", DEGREE(safe_acos(dotProduct(v2, r2))));
    printf("%f\n", DEGREE(safe_acos(dotProduct(v2, n2))));
    printf("%f\n", DEGREE(safe_acos(dotProduct(r2, n2))));
    printf("--<<\n");
#endif
    v2.normalize();
    r2.normalize();
    n2.normalize();

    /* vertex and planeNormal lie in a plane orthognal to
    rotation axis.

    Find rotation in x/y plane (around z) which brings vertex perpendicular
	plane normal (minimum rotation angle).   
  */
    alpha = v2.angle(n2);                       /* no rotation direction given */

    if(alpha <= PIH)
        alpha2 = PIH - alpha;
    else
        alpha2 = alpha - PIH;

    cosa = cos(alpha2);
    sina = sin(alpha2);
    testV[0] = cosa * v2[0] - sina * v2[1];
    testV[1] = sina * v2[0] + cosa * v2[1];

    if(fabs(testV[0] * n2[0] + testV[1] * n2[1]) < diffZeroTol)
        alpha = alpha2;
    else if(alpha <= PIH)
        alpha = alpha - PIH;
    else
        alpha = PIH - alpha;

    rotZ.rot(DEGREE(alpha), ROT_Z);
    v2 = rotZ * v2;

    //v2.dump();
    transf.inverse(itransf);

    *vertex = itransf * v2;

    //vertex->dump();
    return 0;
}

#if 0

/* */
int triangleEdgeLengths(int triangleId, TriangleSet *mesh, double *lengths)
{
    int         i;
    TinyVector  v[3];

    for(i = 0; i < 3; i++)
    {
        v[i] = mesh->getVertex(mesh->getTriangleVertex(triangleId, i));
    }

    lengths[0] = abs(v[1] - v[0]);
    lengths[1] = abs(v[2] - v[1]);
    lengths[2] = abs(v[0] - v[2]);

    return 0;
}

/* point p, line a+b*t */
double distancePointFromLine(TinyVector p, TinyVector a, TinyVector b)
{
    return abs(b * (p - a)) / abs(b);
}

/* */
TinyMatrix rotateVectorOntoZ(TinyVector a)
{
    TinyVector  p21, p22, p23;
    TinyVector  segPlaneNormal;
    TinyMatrix  rotx, roty, rotz, tall, tinv;

    double      cosv, sinv, cosw, sinw, cosx, sinx;
    double      d1, d2, d3;
    double      alpha, sinAlpha;
    double      g, x, y;
    int         negY;
    double      dir;

    p21 = a;

    d1 = sqrt((p21[2] * p21[2]) + (p21[0] * p21[0]));

    if(d1 >= 0.00001)                           /* ok ? */
    {
        cosv = p21[2] / d1;
        sinv = p21[0] / -d1;
        roty = TinyMatrix(cosv, 0, sinv, 0, 0, 1, 0, 0, -sinv, 0, cosv, 0, 0,
                          0, 0, 1);
    }
    else
    {
        roty = TinyMatrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    }

    p22 = roty * p21;

    cosw = p22[2];
    sinw = p22[1];
    rotx = TinyMatrix(1, 0, 0, 0, 0, cosw, -sinw, 0, 0, sinw, cosw, 0, 0, 0, 0,
                      1);
    p23 = rotx * p22;

    tall = rotx * roty;

    //tinv = tall;
    //tinv.inverse (tinv);
    return tall;
}

/* rotate vector a onto vector b, return resulting transformation matrix */
TinyMatrix rotateVectorOntoVector(TinyVector a, TinyVector b)
{
    TinyMatrix  t1, t2, t2inv, tall;

    t1 = rotateVectorOntoZ(a);
    t2 = rotateVectorOntoZ(b);

    t2inv = t2;
    t2inv.inverse(t2inv);
    tall = t2inv * t1;

    return tall;
}

/* */
void printMatrix(TinyMatrix &m)
{
    int i, j;

    printf("---------------------\n");
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            printf("%f ", m.element(i, j));
        }

        printf("\n");
    }

    printf("---------------------\n");
}

/* rotate two connected points (arc) on surface of (unit) sphere onto 
  another two points (arc): A-B -> C-D (A matches C, B matches D) */
int rotateArcOntoArc(TinyVector a, TinyVector b, TinyVector c, TinyVector d,
                     TinyMatrix *m)
{
    TinyMatrix  m1, m2;
    double      l, rangle;
    TinyVector  rc, h1, h2;
    RRotation   rot;

    /* first rotate a on c */
    m1 = rotateVectorOntoVector(a, c);
    b = m1 * b;

    //std::cout << "b=" << b << "\n";
    //std::cout << "c=" << c << "\n";
    /* next rotate b around axis c(a) onto d */
    /* determine center of rotation */
    l = dotProduct(b, c);

    if(l < 0.00001)                             /* ok ? */
    {
        rc = TinyVector(0, 0, 0);
    }
    else
    {
        /* normalize c to length l */
        //rc=c * (1/(l*l));
        rc = c * l;
    }

    //std::cout << "rc=" << rc << "\n";
    /* determine rotation angle */
    h1 = d - rc;
    h2 = b - rc;
    rangle = safe_acos(dotProduct(h1, h2) / (abs(h1) * abs(h2)));
    rangle = (rangle / pi) * 180.0;             /* radians -> degree */

    rot = RRotation(c, rc, rangle);
    m2 = rot.matrix();

    //printMatrix(m1);
    //printMatrix(m2);
    *m = (m2 * m1);

    return 0;
}

/* rotates p2 onto z, p3 into z,y plane, transforms p1 into center */
int rotateTriangle(TinyVector p1, TinyVector p2, TinyVector p3, TinyMatrix *m)
{
    TinyMatrix  trans1, roty, rotx, rotz;
    TinyVector  p11, p21, p31;
    TinyVector  p12, p22, p32;
    TinyVector  p13, p23, p33;

    double      sina, cosa;
    double      d1, d2, d3;
    double      angle;

    /* translate p1 to origin */
    trans1 = RTranslation(-p1[0], -p1[1], -p1[2]).matrix();

    p11 = trans1 * p1;
    p21 = trans1 * p2;
    p31 = trans1 * p3;

    //std::cout << "p1'=" << p11 << ",p2'=" << p21 << ",p3'=" << p31 << "\n";
    d1 = sqrt(p21[2] * p21[2] + p21[0] * p21[0]);

    if(d1 < 0.00001)
    {
        cosa = 0;
        sina = 1;
    }
    else
    {
        cosa = p21[2] / d1;
        sina = -(p21[0] / d1);
    }

    roty = TinyMatrix(cosa, 0, sina, 0, 0, 1, 0, 0, -sina, 0, cosa, 0, 0, 0, 0,
                      1);

    p12 = p11;
    p22 = roty * p21;
    p32 = roty * p31;

    //std::cout << "p1''=" << p12 << ",p2''=" << p22 << ",p3''=" << p32 << "\n";
    d2 = abs(p2 - p1);

    if(d2 < 0.00001)
    {
        cosa = 0;
        sina = 1;
    }
    else
    {
        cosa = p22[2] / d2;
        sina = p22[1] / d2;
    }

    rotx = TinyMatrix(1, 0, 0, 0, 0, cosa, -sina, 0, 0, sina, cosa, 0, 0, 0, 0,
                      1);

    p13 = p12;
    p23 = rotx * p22;
    p33 = rotx * p32;

    //std::cout << "p1'''=" << p13 << ",p2'''=" << p23 << ",p3'''=" << p33 << "\n";
    d3 = sqrt((p33[0] * p33[0]) + (p33[1] * p33[1]));

    if(d3 < 0.00001)
    {
        cosa = 0;
        sina = 1;
    }
    else
    {
        cosa = p33[1] / d3;
        sina = p33[0] / d3;
    }

    TinyVector  tv1, tv2;

    tv1 = TinyVector(1, -0.707107, 0);
    tv1.normalize();
    tv2 = TinyVector(0, 1, 0);

    rotz = TinyMatrix(cosa, -sina, 0, 0, sina, cosa, 0, 0, 0, 0, 1, 0, 0, 0, 0,
                      1);

    *m = rotz * rotx * roty * trans1;

    return 0;
}

/* */
double det2(TinyMatrix &m)
{
    return m.element(0, 0) * m.element(1, 1) - m.element(1, 0) * m.element(0, 1);
}

/* */
double det3(TinyMatrix &m)
{
    double  hd, nd;

    /* apply sarrus rule */
    hd = 0;
    nd = 0;

    hd += (m.element(0, 0) * m.element(1, 1) * m.element(2, 2));
    hd += (m.element(1, 0) * m.element(2, 1) * m.element(0, 2));
    hd += (m.element(2, 0) * m.element(0, 1) * m.element(1, 2));

    nd += (m.element(0, 2) * m.element(1, 1) * m.element(2, 0));
    nd += (m.element(1, 2) * m.element(2, 1) * m.element(0, 0));
    nd += (m.element(2, 2) * m.element(0, 1) * m.element(1, 0));

    return hd - nd;
}

/* */
MList<int> *removeDuplicateTriangles(MList<int> *l, TriangleSet *mesh)
{
    int             ta[MAX_NUM_TRIANGLES], i, j, n, h;
    MIterator<int>  *it;
    MList<int>      *nl;
    int             vIds[MAX_NUM_TRIANGLES][3];

    /* copy list into array, then compare */
    it = l->newIterator();
    nl = new MList < int > ;

    i = 0;
    while(it->valid())
    {
        ta[i] = it->current();
        vIds[i][0] = mesh->getTriangleVertex(ta[i], 0);
        vIds[i][1] = mesh->getTriangleVertex(ta[i], 1);
        vIds[i][2] = mesh->getTriangleVertex(ta[i], 2);

        /* sort vertex ids in ascending order */
        if(vIds[i][0] > vIds[i][1])
        {
            h = vIds[i][0];
            vIds[i][0] = vIds[i][1];
            vIds[i][1] = h;
        }

        if(vIds[i][1] > vIds[i][2])
        {
            h = vIds[i][1];
            vIds[i][1] = vIds[i][2];
            vIds[i][2] = h;
        }

        if(vIds[i][0] > vIds[i][1])
        {
            h = vIds[i][0];
            vIds[i][0] = vIds[i][1];
            vIds[i][1] = h;
        }

        i++;
        it->next();
    }

    n = i;

    for(i = 0; i < n - 1; i++)
    {
        for(j = i + 1; j < n; j++)
        {
            if((ta[i] != -1) &&
               (vIds[i][0] == vIds[j][0]) &&
               (vIds[i][1] == vIds[j][1]) &&
               (vIds[i][2] == vIds[j][2]))
            {
                ta[j] = -1;
            }
        }

        if(ta[i] != -1)
            nl->append(ta[i]);
    }

    /* BUGFIX: last element was always removed */
    if(ta[n - 1] != -1)
        nl->append(ta[n - 1]);

    it->~MIterator();
    l->~MList();

    return nl;
}

/* */
int lotPointLine(TinyVector p, TinyVector lp, TinyVector lrv, TinyVector *result)
{
    double  alpha;
    double  z, n;

    n = lrv[0] * lrv[0] + lrv[1] * lrv[1] + lrv[2] * lrv[2];
    if(n < 0.000001)
        return -1;

    z = lrv[0] * (p[0] - lp[0]) + lrv[1] * (p[1] - lp[1]) + lrv[2] * (p[2] - lp[2]);
    alpha = z / n;
    (*result)[0] = lp[0] + alpha * lrv[0];
    (*result)[1] = lp[1] + alpha * lrv[1];
    (*result)[2] = lp[2] + alpha * lrv[2];

    return 0;
}

/* */
int lotPointPlane(TinyVector p, GEOMARK_Plane *plane, TinyVector *result)
{
    TinyVector  lot, n;
    double      d;

    n = plane->normal;
    d = plane->distToOrigin;

    lot = p + (d - (dotProduct(n, p)) / dotProduct(n, n)) * n;

    *result = lot;

    return 0;
}

/* */
int triangleCoordSystem(TinyVector a, TinyVector b, TinyVector c,
                        TriangleCoordSystem *coords)
{
    TinyVector  center;
    TinyVector  lot;

    center = (a + b + c) / 3.;
    lotPointLine(c, a, b - a, &lot);

    coords->center = center;
    coords->xDir = b - lot;
    coords->xDir.normalize();
    coords->yDir = c - lot;
    coords->yDir.normalize();

    return 0;
}

/* */
int world2TriangleCoords(TriangleCoordSystem *coords, TinyVector p,
                         TinyVector *result)
{
    TinyVector  lotx, loty;
    TinyVector  dirTest;
    double      xRel, yRel;

    lotPointLine(p, coords->center, coords->xDir, &lotx);
    lotPointLine(p, coords->center, coords->yDir, &loty);

    lotx = p - lotx;
    loty = p - loty;

    dirTest = lotx;
    dirTest.normalize();
    if(dotProduct(coords->yDir, dirTest) < 0)
        yRel = abs(lotx) * -1.0;
    else
        yRel = abs(lotx);

    dirTest = loty;
    dirTest.normalize();
    if(dotProduct(coords->xDir, dirTest) < 0)
        xRel = abs(loty) * -1.0;
    else
        xRel = abs(loty);
    (*result)[0] = xRel;
    (*result)[1] = yRel;
    (*result)[2] = 0.;

    return 0;
}

/* */
int triangle2WorldCoords(TriangleCoordSystem *coords, TinyVector p,
                         TinyVector *result)
{
    *result = coords->center + coords->xDir * p[0] + coords->yDir * p[1];

    return 0;
}

/* */
int rotCoordSystem(TinyVector lp1, TinyVector lp2, TinyVector p,
                   RotCoordSystem *coords)
{
    TinyVector  lot;

    lotPointLine(p, lp1, lp2 - lp1, &lot);      /* bug */

    //std::cout << "lot=" << lot << "\n";
    coords->center = lot;
    coords->yDir = lp1 - lot;
    coords->yDir.normalize();
    coords->xDir = p - lot;
    coords->xDir.normalize();

    /* Rechts-System (positiv orientiert) */
    coords->zDir = coords->xDir * coords->yDir; /* coords->zDir.normalize(); */

    coords->xPlane.center = coords->center;
    coords->xPlane.distToOrigin = dotProduct(coords->xDir, coords->center);
    coords->xPlane.normal = coords->xDir;

    coords->yPlane.center = coords->center;
    coords->yPlane.distToOrigin = dotProduct(coords->yDir, coords->center);
    coords->yPlane.normal = coords->yDir;

    coords->zPlane.center = coords->center;
    coords->zPlane.distToOrigin = dotProduct(coords->zDir, coords->center);
    coords->zPlane.normal = coords->zDir;

    return 0;
}

/* */
int world2RotCoords(RotCoordSystem *coords, TinyVector p, TinyVector *result)
{
    TinyVector  lotx, loty, lotz, dirTest;
    double      xRel, yRel, zRel;

    lotPointPlane(p, &coords->xPlane, &lotx);
    lotPointPlane(p, &coords->yPlane, &loty);
    lotPointPlane(p, &coords->zPlane, &lotz);

    lotx = p - lotx;
    loty = p - loty;
    lotz = p - lotz;

    dirTest = lotx;
    dirTest.normalize();
    if(dotProduct(coords->xDir, dirTest) < 0)
        xRel = abs(lotx) * -1.0;
    else
        xRel = abs(lotx);

    dirTest = loty;
    dirTest.normalize();
    if(dotProduct(coords->yDir, dirTest) < 0)
        yRel = abs(loty) * -1.0;
    else
        yRel = abs(loty);

    dirTest = lotz;
    dirTest.normalize();
    if(dotProduct(coords->zDir, dirTest) < 0)
        zRel = abs(lotz) * -1.0;
    else
        zRel = abs(lotz);
    (*result)[0] = xRel;
    (*result)[1] = yRel;
    (*result)[2] = zRel;

    return 0;
}

/* */
int rot2WorldCoords(RotCoordSystem *coords, TinyVector p, TinyVector *result)
{
    *result = coords->center +
        coords->xDir *
        p[0] +
        coords->yDir *
        p[1] +
        coords->zDir *
        p[2];

    return 0;
}

/* */
int adjustVector(TinyVector lp1, TinyVector lp2, TinyVector p, TinyVector z,
                 double length, TinyVector *result)
{
    RotCoordSystem  coords;
    TinyVector      pr, zr;
    double          k1, k2, k3, h;
    double          a, b, c;
    double          x1, x2;
    double          alpha[4], cosAlpha1, cosAlpha2;
    int             i;
    RRotation       rot;
    TinyVector      pr2;
    double          dist, minDist, distToZ;
    TinyVector      prBest;

    rotCoordSystem(lp1, lp2, p, &coords);
    world2RotCoords(&coords, p, &pr);
    world2RotCoords(&coords, z, &zr);

    //std::cout << "pr=" << pr << "\n";
    //std::cout << "zr=" << zr << "\n";
    //std::cout << "abs(pr-zr)=" << abs(pr-zr) << "\n";
    h = (pr[0] * pr[0]) + (pr[1] * pr[1]) + (pr[2] * pr[2]) + (zr[0] * zr[0]) + (zr[1] * zr[1]) + (zr[2] * zr[2]) - 2. * pr[1] * zr[1];

    k1 = -2. * pr[2] * zr[0] + 2. * pr[0] * zr[2];
    k2 = -2. * pr[0] * zr[0] - 2. * pr[2] * zr[2];
    k3 = (length * length) - h;

    a = (k1 * k1) - (k2 * k2);
    b = -2. * k1 * k3 + (k2 * k2);
    c = (k3 * k3) - (k2 * k2);

    if(a == 0)
    {
        return -1;
    }

    x1 = (-1. * b + sqrt((b * b) - 4 * a * c)) / (2 * a);
    x2 = (-1. * b - sqrt((b * b) - 4 * a * c)) / (2 * a);

    cosAlpha1 = sqrt(1 - (x1 * x1));
    cosAlpha2 = sqrt(1 - (x2 * x2));

    if((fabs(cosAlpha1) > 1) || (fabs(cosAlpha2) > 1))
    {
        return -1;
    }

    alpha[0] = (acos(cosAlpha1) / pi) * 180.;
    alpha[1] = (acos(-1. * cosAlpha1) / pi) * 180.;
    alpha[2] = (acos(cosAlpha2) / pi) * 180.;
    alpha[3] = (acos(-1. * cosAlpha2) / pi) * 180.;

    minDist = -1.;
    for(i = 0; i < 4; i++)
    {
        rot = RRotation(TinyVector(0, 1, 0), TinyVector(0, 0, 0), alpha[i]);
        pr2 = rot.matrix() * pr;
        dist = abs(pr2 - pr);
        distToZ = abs(pr2 - zr);
        if(((minDist == -1.) || (dist < minDist)) &&
           (fabs(distToZ - length) < 0.00001))  /* ok ? */
        {
            prBest = pr2;
            minDist = dist;
        }
    }

    if(minDist == 2.)
    {
        return -1;
    }

    rot2WorldCoords(&coords, prBest, result);

    return 0;
}

/* print edge lengths of faces */
void dumpModelLengthsAndAngles(TriangleSet *mesh)
{
    int         i, j, n;
    TinyVector  v1, v2, v3;
    TinyVector  com;

    printf("** model dump **\n");

    com = centerOfMass(mesh);

    n = mesh->triangles();
    for(i = 0; i < n; i++)
    {
        v1 = mesh->getVertex(mesh->getTriangleVertex(i, 0));
        v2 = mesh->getVertex(mesh->getTriangleVertex(i, 1));
        v3 = mesh->getVertex(mesh->getTriangleVertex(i, 2));
        printf("triangle %d\n", i);
        printf("  %f,%f,%f\n", abs(v2 - v1), abs(v3 - v2), abs(v1 - v3));
        printf("  %f,%f,%f\n", abs(v1 - com), abs(v2 - com), abs(v3 - com));
    }
}

/* */
int invert2(TinyMatrix *m, TinyMatrix *result)
{
    double      D;
    TinyMatrix  rm;

    D = (*m)[0][0] * (*m)[1][1] - (*m)[0][1] * (*m)[1][0];

    if(D == 0)
        return -1;

    if(global_debug)
        printf("D=%f\n", D);

    D = 1 / D;

    rm[0][0] = (*m)[1][1] * D;
    rm[0][1] = -(*m)[0][1] * D;
    rm[1][0] = -(*m)[1][0] * D;
    rm[1][1] = (*m)[0][0] * D;

    *result = rm;

    return 0;
}
#endif
#endif
