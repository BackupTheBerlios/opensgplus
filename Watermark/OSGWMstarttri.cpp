#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#define QUIET

#ifndef QUIET
#define GEOMARK_DBG
#define GEOMARK_PRINTF  printf
#else
#define GEOMARK_DBG     / ## /
#define GEOMARK_PRINTF  / ## /
#endif
#include "OSGWMstarttri.h"

static const double ZERO_TOL = 1e-06;

double              fixedRatios[NUM_FIXED_RATIOS][2] =
{                       /* already normalized */
    1.5,
    2.,
    2.,
    2.,
    2.,
    2.5
};

/* */

int normalizeRatios(double *r)
{
    double  h;
    int     i, j;

    /* sort in ascending order */
    for(i = 0; i < 2; i++)
    {
        for(j = i + 1; j < 3; j++)
        {
            if(r[i] > r[j])
            {
                h = r[i];
                r[i] = r[j];
                r[j] = h;
            }
        }
    }

    /* BUGFIX */
    if(fabs(r[0]) > ZERO_TOL)
    {
        /* normalize edge lengths with respect to shortest edge */
        for(i = 1; i < 3; i++)
            r[i] = r[i] / r[0];
        r[0] = 1.;
    }
    else
        return -1;

    return 0;
}

/* for a "valid" (=constructable) triangle with edge lengths a,b,c the triangle
  inequality must be fulfilled: a+b<=c  */
BOOL validTriangleEdgeLengths(double a, double b, double c)
{
    if(a + b <= c)
        return FALSE;
    else if(b + c <= a)
        return FALSE;
    else if(a + c <= b)
        return FALSE;

    return TRUE;
}

/* calculate average distance of adjacent vertices 
  we use meanValue of all three edge lengths of each triangle
  alpha means alpha-quantil: e.g. 0.25 for 25% quantil.
*/
int calcVertexDist(TriangleSet *mesh, double *meanDistance, double alpha,
                   double *quantilValue)
{
    int         i, j, n, index;

    //SortEntry trVertexDist[MAX_NUM_FACES];
    SortEntry   *trVertexDist;
    double      meanValue;
    double      lengths[3], meanLength, sum;

    meanValue = 0;

    // get number of triangles contained in the mesh
    n = mesh->triangles();

    meanLength = 0;

    trVertexDist = (SortEntry *) malloc(n * sizeof(SortEntry));

    for(i = 0; i < n; i++)
    {
        // get edge lengths of triangle with triangle index i in mesh
        triangleEdgeLengths(i, mesh, lengths);

        // get perimeter of triangle	
        sum = lengths[0] + lengths[1] + lengths[2];

        meanLength = meanLength + sum;
        trVertexDist[i].origIndex = i;

        // mean edge length of triangle
        trVertexDist[i].value = sum / 3;
    }

    // mean edge length of triangles in mesh
    *meanDistance = meanLength / (3 * n);

    /* sort vertex distances in ascending order */
    qsort((void *) trVertexDist, (size_t) n, (size_t) sizeof(SortEntry),
          compareSortEntries);

    /* calculate index corresponding to quantil */
    index = (int) (fabs(alpha * (double) n));

    // mean edge length of triangle at index corresponding to quantil.
    *quantilValue = trVertexDist[index].value;

    GEOMARK_FREE(trVertexDist);
    trVertexDist = NULL;

    //  GEOMARK_PRINTF("calcVertexDist done\n");
    return 0;
}

/* */
int allocEdgeRatioResultArray(EdgeRatioResult **result, int numTriangles)
{
    int i;

    *result = (EdgeRatioResult *) malloc(NUM_FIXED_RATIOS * sizeof(EdgeRatioResult));
    for(i = 0; i < NUM_FIXED_RATIOS; i++)
    {
        (*result)[i].numMatches = 0;
        (*result)[i].matches = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(numTriangles),
                                                 sizeof(TriangleEdgeRatioMatch));
    }

    for(i = 0; i < NUM_FIXED_RATIOS; i++)
    {
        (*result)[i].origIndex = i;
        (*result)[i].numMatches = 0;
        (*result)[i].numAcceptedMatches = 0;
        (*result)[i].matches->empty();
    }

    return 0;
}

/* */
int emptyEdgeRatioResultArray(EdgeRatioResult *result)
{
    int i;

    for(i = 0; i < NUM_FIXED_RATIOS; i++)
    {
        result[i].origIndex = i;
        result[i].numMatches = 0;
        result[i].numAcceptedMatches = 0;
        result[i].matches->empty();
    }

    return 0;
}

/* */
int freeEdgeRatioResultArray(EdgeRatioResult *result)
{
    int i;

    for(i = 0; i < NUM_FIXED_RATIOS; i++)
    {
        GEOMARK_DELETE(result[i].matches);
        result[i].matches = NULL;
    }

    GEOMARK_FREE(result);
    result = NULL;

    return 0;
}

/* */
int compareTriangleEdgeRatio(const void *elem1, const void *elem2)
{
    TriangleEdgeRatioMatch  *e1, *e2;

    e1 = (TriangleEdgeRatioMatch *) elem1;
    e2 = (TriangleEdgeRatioMatch *) elem2;

    if((e1->isAccepted) && (!e2->isAccepted))
        return -1;
    else if((!e1->isAccepted) && (e2->isAccepted))
        return 1;
    else if(e1->diffFromRatio < e2->diffFromRatio)
        return -1;
    else if(e1->diffFromRatio == e2->diffFromRatio)
        return 0;
    else
        return 1;
}

/* */
int compareEdgeRatioResult(const void *elem1, const void *elem2)
{
    EdgeRatioResult *e1, *e2;

    e1 = (EdgeRatioResult *) elem1;
    e2 = (EdgeRatioResult *) elem2;

    if((e1->numAcceptedMatches == 0) && (e2->numAcceptedMatches > 0))
        return 1;
    else if((e1->numAcceptedMatches > 0) && (e2->numAcceptedMatches == 0))
        return -1;
    else if(e1->numMatches < e2->numMatches)
        return -1;
    else if(e1->numMatches == e2->numMatches)
        return 0;
    else
        return 1;
}

/* toleratedDiff must be choosen so fixed ratios do not overlap !
*/
int scanTrianglesForFixedEdgeRatios(TriangleSet *mesh, int maxNumMatches,
                                    double toleratedDiff,
                                    EdgeRatioResult *result,
                                    double avgEdgeLengthQuantil,
                                    BOOL checkAvgLength)
{
    int                     i, j, n, numElements;
    TriangleEdgeRatioMatch  trMatch;
    double                  dmax, lengths[3], lengthMeanValue;
    BOOL                    match;
    double                  d;
    int                     rv;

    assert(result != NULL);

    dmax = toleratedDiff;

    n = mesh->triangles();
    for(i = 0; i < n; i++)
    {
        triangleEdgeLengths(i, mesh, lengths);
        lengthMeanValue = (lengths[0] + lengths[1] + lengths[2]) / 3.;

        rv = normalizeRatios(lengths);

        if(!rv)
        {
            match = FALSE;
            j = 0;
            while((!match) && (j < NUM_FIXED_RATIOS))
            {
                d = MAX2(fabs((lengths[1] / fixedRatios[j][0]) - 1),
                         fabs((lengths[2] / fixedRatios[j][1]) - 1));

                if(d < dmax)
                {
                    match = TRUE;

                    trMatch.triangleId = i;
                    trMatch.lengthMeanValue = lengthMeanValue;
                    trMatch.diffFromRatio = d;
                    result[j].numMatches++;

                    if((!checkAvgLength) ||
                       (avgEdgeLengthQuantil > lengthMeanValue))
                    {   /* not optimal, change later ... */
                        result[j].numAcceptedMatches++;
                        trMatch.isAccepted = TRUE;
                    }
                    else
                        trMatch.isAccepted = FALSE;
                    result[j].matches->appendElement(&trMatch);
                }

                j++;
            }
        }               /* if !rv */
    }                   /* for i<mesh->triangles() */

    /* sort Edge Ratio Results based on numMatches. if numAcceptedMatches is
    0, entry is sorted to the end.
  */
    qsort((void *) result, (size_t) NUM_FIXED_RATIOS,
          (size_t) sizeof(EdgeRatioResult), compareEdgeRatioResult);

    /* sort matched triangles based on edge ratio difference if
    numAcceptedMatches is > 0. Not accepted matches are sorted to the end.
  */
    for(i = 0; i < NUM_FIXED_RATIOS; i++)
    {
        numElements = result[i].matches->getNumElements();

        if(numElements > 0)
        {
            result[i].matches->sort(0, -1, compareTriangleEdgeRatio);
        }
    }

    /* dump results */
    for(i = 0; i < NUM_FIXED_RATIOS; i++)
    {
        numElements = result[i].matches->getNumElements();

        /*    printf("results for fixed ratio no. %d\n",i);
    printf("numMatches=%d\n",result[i].numMatches);
    printf("numAcceptedMatches=%d\n",result[i].numAcceptedMatches);
    printf("numElements=%d\n",numElements);
*/
        if(numElements > 0)
        {
            for(j = 0; j < numElements; j++)
            {
                trMatch = *((TriangleEdgeRatioMatch *) result[i].matches->getElement(j));
            }
        }
    }

    return 0;
}

// Find indices of triangles (within original mesh) with edge length ratio
// fulfilling the error criterium:
// Distance of quotient of ratio to optimal ratio is below dmax.
// Result is returned within in/out parameter trIds: Entries sorted by
// in acending order of distance.
int scanTrianglesForEdgeRatio(double *ratio, TriangleSet *mesh,
                              SortEntry *trIds, int *numMatches,
                              int maxNumMatches, double toleratedDiff,
                              BOOL checkRatio)
{
    double  d, dmax;
    int     i, j, n, nt;
    double  lengths[3], lengthMeanValue;
    int     rv;

    normalizeRatios(ratio);

    /* check for triangle-inequality */
    if(!validTriangleEdgeLengths(ratio[0], ratio[1], ratio[2]))
        return -1;

    /* tolerated diff means percentage by which absolute edge length value is
    allowed to change */
    /* check if tolerated diff is choosen too large
     -> ordered sequence of ratios is not allowed to change
     -> with diff changes for each vertex result always in a "valid" triangle
  */
    dmax = 1;

    /* 
    Calculate smallest modification of the ratios
    which may result in an invalid ratio.

    -> this is just for informational purposes, so
       in the normal case checkRatio is FALSE 
  */
    if(checkRatio)
    {
        d = 0.5 * (-2 + sqrt(4 - 4 * (1 - ratio[1] / ratio[0])));
        if(d < dmax)
            dmax = d;
        d = (ratio[2] - ratio[1]) / (ratio[2] + ratio[1]);
        if(d < dmax)
            dmax = d;
        d = sqrt((ratio[0] + ratio[1] - ratio[2]) / ratio[0]);
        if(d < dmax)
            dmax = d;
    }

    if(toleratedDiff < dmax)
        dmax = toleratedDiff;

    if(dmax == 0)
        return -1;      /* not likely to find a matching triangle */

    n = mesh->triangles();
    nt = 0;
    i = 0;

    // Find all triangles with edge length ratio fulfilling the error criterium:
    // Distance of quotient of ratio and optimal ratio is below dmax.
    while((i < n) && (nt < maxNumMatches))
    {
        // Edge lengths of current triangle
        triangleEdgeLengths(i, mesh, lengths);

        lengthMeanValue = (lengths[0] + lengths[1] + lengths[2]) / 3.;

        // Sort edge lengths within triangle in ascending order and normalize
        // edge lengths with respect to shortest edge.
        rv = normalizeRatios(lengths);

        if(!rv)
        {               // Get out if edge length of shortest edge is not beyond ZERO_TOL.
            // Calc quotient of actual ratio and required ratio and its distance to
            // the optimal value (which is 1 of course).
            d = MAX2(fabs((lengths[1] / ratio[1]) - 1),
                     fabs((lengths[2] / ratio[2]) - 1));

            // WF 030724: What does BUGFIX mean here? 2nd condition actually never happens:
            // lengths[0] is the shortest edge within the triangle and is always GT ZERO_TOL.
            // The other edges are longer than lengths[0] by definition.
            /* BUGFIX */
            if((d < dmax) &&
               (
                   fabs(lengths[0]) +
               fabs(lengths[1]) +
               fabs(lengths[2]) > 3. *
               ZERO_TOL
           ))
            {
                trIds[nt].origIndex = nt;
                trIds[nt].value = d;
                trIds[nt].gp1 = i;
                trIds[nt].gp2 = lengthMeanValue;

                nt++;
            }
        }               /* if !rv */

        i++;
    }

    *numMatches = nt;

    if(nt > 0)
    {
        /* sort matching triangles in ascending order of distances to optimal ratio (c.f. above) */
        qsort((void *) trIds, (size_t) nt, (size_t) sizeof(SortEntry),
              compareSortEntries);
    }

    return 0;
}

/* change triangle so it matches a given ratio */

/*
  what is constant:

  variant==CHANGE_RATIO_KEEP_COM_CONSTANT:

  - length of longest edge (WF 030726: Incorrect. The smallest edge of the 
    original triangle will be the longest edge of the new triangle. Intended?)
  - center of mass
  - triangle normal

  variant==CHANGE_RATIO_KEEP_LONGEST_EDGE_CONSTANT:

  - length of longest edge
  - vertices adjacent to longest edge
  - triangle normal
*/
int changeRatio(double *ratio, TriangleSet *mesh, int trId, int variant,
                BOOL doNotChange, TinyVector *resultVectors)
{
    double      s, r, a, b, c;
    double      alpha, beta, gamma;
    double      h, p;
    double      lengths[3], dsav;
    TinyVector  v[3];
    int         i, j;
    int         vIds[3], index[3], isav, index2[3];
    TinyVector  n1, n2, sp, com;
    double      scaleFactor;
    double      comX, comY;
    double      rvA[2], rvB[2], rvC[2];
    TinyVector  newVertices[3];
    BOOL        edgeACisShortest;

    normalizeRatios(ratio);

    if(!validTriangleEdgeLengths(ratio[0], ratio[1], ratio[2]))
        return -1;

    a = ratio[0];
    b = ratio[1];
    c = ratio[2];

    // ----------------------------------------------------------
    // Calculate properties of the "ratio triangle".
    // Triangle is supposed lie with A in origin, c on x-axis
    // ----------------------------------------------------------
    /* half of triangle border length */
    s = (a + b + c) / 2;

    /* radius of inner circle */
    r = sqrt(((s - a) * (s - b) * (s - c)) / s);

    /* angles */
    alpha = 2 * atan(r / (s - a));
    beta = 2 * atan(r / (s - b));
    gamma = 2 * atan(r / (s - c));

    h = sin(alpha) * b;
    p = cos(alpha) * b;

    /* Retrieve vertices of triangle to be changed */
    for(i = 0; i < 3; i++)
    {
        vIds[i] = mesh->getTriangleVertex(trId, i);
        v[i] = mesh->getVertex(vIds[i]);
    }

    // Original edge lengths
    lengths[0] = tv_abs(v[0] - v[1]);
    lengths[1] = tv_abs(v[1] - v[2]);
    lengths[2] = tv_abs(v[2] - v[0]);

    for(i = 0; i < 3; i++)
        index[i] = i;

    // Sort original edge lenghts in ascending order
    for(i = 0; i < 2; i++)
    {
        for(j = i + 1; j < 3; j++)
        {
            if(lengths[i] > lengths[j])
            {
                dsav = lengths[i];
                lengths[i] = lengths[j];
                lengths[j] = dsav;
                isav = index[i];
                index[i] = index[j];
                index[j] = isav;
            }
        }
    }

    /*
  For constructing the new start triangle from the ratio triangle, 
  sort vertices so that edge from first to second (c) is longest,
  second to third (a) is shortest edge.
  This configuration matches the ratio triple configuration.
  a < b < c (e.g. 1:1.5:2).
  */
    if((index[2] == 1) && (index[0] == 0))
    {
        /* 2->1->0 */
        index2[0] = 2;
        index2[1] = 1;
        index2[2] = 0;
    }
    else if((index[2] == 0) && (index[0] == 1))
    {
        /* 0->1->2 */
        index2[0] = 0;
        index2[1] = 1;
        index2[2] = 2;
    }
    else if((index[2] == 1) && (index[0] == 2))
    {
        /* 1->2->0 */
        index2[0] = 1;
        index2[1] = 2;
        index2[2] = 0;
    }
    else if((index[2] == 2) && (index[0] == 1))
    {
        /* 0->2->1 */
        index2[0] = 0;
        index2[1] = 2;
        index2[2] = 1;
    }
    else if((index[2] == 0) && (index[0] == 2))
    {
        /* 1->0->2 */
        index2[0] = 1;
        index2[1] = 0;
        index2[2] = 2;
    }
    else if((index[2] == 2) && (index[0] == 0))
    {
        /* 2->0->1 */
        index2[0] = 2;
        index2[1] = 0;
        index2[2] = 1;
    }

    n1 = v[index2[1]] - v[index2[0]];

    lotPointLine(v[index2[2]], v[index2[1]], v[index2[0]] - v[index2[1]], &sp);
    n2 = v[index2[2]] - sp;

    n1.normalize();
    n2.normalize();

    // WF 030801: Largest edge length value and maximum of ratio.
    // This means the longest edge of the original triangle
    // will be the longest edge of the new triangle.
    scaleFactor = lengths[index[2]] / ratio[2];

    //scaleFactor=lengths[index[0]] / ratio[2]; // maintains smallest edge of original.
    //scaleFactor=lengths[index[0]];
    if(variant == CHANGE_RATIO_KEEP_COM_CONSTANT)
    {
        /* calculate center of mass of "ratio-triangle" */
        comX = (c + p) / 3;
        comY = h / 3;

        /* center of mass of real triangle remains the same */
        /* PRECONDITION:
		edge AB (aka edge c) must be longest, BC (aka edge a) shortest !! */
        /* c is longest,a shortest */
        rvA[0] = -comX;
        rvA[1] = -comY;

        rvB[0] = c - comX;
        rvB[1] = -comY;

        rvC[0] = p - comX;
        rvC[1] = h - comY;

        com = triangleCenter(trId, mesh);

        /* New vertices A,B,C */
        newVertices[0] = com +
            n1 *
            scaleFactor *
            rvA[0] +
            n2 *
            scaleFactor *
            rvA[1];
        newVertices[1] = com +
            n1 *
            scaleFactor *
            rvB[0] +
            n2 *
            scaleFactor *
            rvB[1];
        newVertices[2] = com +
            n1 *
            scaleFactor *
            rvC[0] +
            n2 *
            scaleFactor *
            rvC[1];

        /* replace vertices in mesh with new ones */
        for(i = 0; i < 3; i++)
        {
            if(!doNotChange)
            {
                mesh->setVertex(vIds[index2[i]], newVertices[i]);
            }
        }

        for(i = 0; i < 3; i++)
        {
            resultVectors[index2[i]] = newVertices[i];
        }
    }                   /* CHANGE_RATIO_KEEP_COM_CONSTANT */
    else if(variant == CHANGE_RATIO_KEEP_LONGEST_EDGE_CONSTANT)
    {
        com = triangleCenter(trId, mesh);

        newVertices[2] = v[index2[0]] +
            n1 *
            p *
            scaleFactor +
            n2 *
            h *
            scaleFactor;

        if(!doNotChange)
        {
            mesh->setVertex(vIds[index2[2]], newVertices[2]);
        }

        resultVectors[index2[2]] = newVertices[2];
        for(i = 0; i < 2; i++)
            resultVectors[i] = v[index2[i]];
    }                   /* CHANGE_RATIO_KEEP_LONGEST_EDGE_CONSTANT */
    else
        return -1;

    com = triangleCenter(trId, mesh);

    triangleEdgeLengths(trId, mesh, lengths);

    double  lengthsNew[3];
    lengthsNew[0] = tv_abs(resultVectors[0] - resultVectors[1]);
    lengthsNew[1] = tv_abs(resultVectors[1] - resultVectors[2]);
    lengthsNew[2] = tv_abs(resultVectors[2] - resultVectors[0]);

    TinyVector  newCom = triangleCenter(resultVectors[0], resultVectors[1],
                                        resultVectors[2]);

    return 0;
}
#endif
