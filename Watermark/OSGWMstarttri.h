#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#ifndef _STARTTRI_H_
#define _STARTTRI_H_

/* change ratio variants */
#define CHANGE_RATIO_KEEP_COM_CONSTANT          1
#define CHANGE_RATIO_KEEP_LONGEST_EDGE_CONSTANT 2

#include "OSGWMtinyvector.h"
#include "OSGWMtriangleset.h"
#include "OSGWMutil.h"

#define NUM_FIXED_RATIOS    3

typedef struct
{
    BOOL    isAccepted;
    int     triangleId;
    double  lengthMeanValue;
    double  diffFromRatio;
} TriangleEdgeRatioMatch;

typedef struct
{
    int         origIndex;  /* original index, needed for accessing fixed ratio after
                 sorting */
    int         numMatches;
    int         numAcceptedMatches;
    GrowArray   *matches;   /* elements of type TriangleEdgeRatioMatch */
} EdgeRatioResult;

int     allocEdgeRatioResultArray(EdgeRatioResult **result, int numTriangles);
int     emptyEdgeRatioResultArray(EdgeRatioResult *result);
int     freeEdgeRatioResultArray(EdgeRatioResult *result);

/* for sorting TriangleEdgeRatioMatch-GrowArray and EdgeRatioResult-Array */
int     compareTriangleEdgeRatio(const void *elem1, const void *elem2);
int     compareEdgeRatioResult(const void *elem1, const void *elem2);

int     normalizeRatios(double *r);
BOOL    validTriangleEdgeLengths(double a, double b, double c);

int     calcVertexDist(TriangleSet *mesh, double *meanDistance, double alpha,
                       double *quantilValue);

int     changeRatio(double *ratio, TriangleSet *mesh, int trId, int variant,
                    BOOL doNotChange, TinyVector *resultVectors);

/* WORK IN PROGRESS */
int     scanTrianglesForFixedEdgeRatios(TriangleSet *mesh, int maxNumMatches,
                                        double toleratedDiff,
                                        EdgeRatioResult *result,
                                        double avgEdgeLengthQuantil,
                                        BOOL checkAvgLength);

int     scanTrianglesForEdgeRatio(double *ratio, TriangleSet *mesh,
                                  SortEntry *trIds, int *numMatches,
                                  int maxNumMatches, double toleratedDiff,
                                  BOOL checkRatio);
#endif
#endif
