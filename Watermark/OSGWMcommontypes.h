#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#ifndef _COMMONTYPES_H_
#define _COMMONTYPES_H_

#include <math.h>   /* for PI */
#include <stdlib.h> /* for free() and NULL */

#ifndef WIN32
#define BOOL    int
#define TRUE    1
#define FALSE   0
#else
#include "OSGWMwtypes.h"
#define MAXPATH 255
#endif
#ifdef WIN32
const double    piVal = 3.14159265358979323846;
#ifndef PI
#define PI  piVal
#endif
#else
#define PI  M_PI
#endif
const double    PIH = PI / 2.;
const double    PI32 = 1.5 * PI;
const double    SQRT2 = sqrt(2.);
const double    SQRT3 = sqrt(3.);

#define MAX2(x, y)          ((x > y) ? x : y)
#define MAX3(x, y, z)       (MAX2(x, MAX2(y, z)))
#define MIN2(x, y)          ((x < y) ? x : y)
#define MIN3(x, y, z)       (MIN2(x, MIN2(y, z)))
#define FRACDIST(x, y)      (fabs(x - y) / (x + y))
#define EVEN(x)             (x % 2 == 0)
#define ODD(x)              (x % 2 != 0)
#define DEGREE(x)           ((x / PI) * 180.0)
#define RADIANS(x)          ((x / 180.0) * PI)
#define ANGLE_TO_DIST(x)    (fabs(2. * sin(x / 2.)))    /* measure angles as distances:
                                                 distance of endpoints of 
                                                 two normalized  vectors 
                                                 with enclosed angle x */

/* GEOMARK_ prefix to avoid name clashes */
#define GEOMARK_FREE(ptr) \
    if(1)                 \
    {                     \
        free(ptr);        \
        ptr = NULL;       \
    }

#define GEOMARK_DELETE(ptr) \
    if(1)                   \
    {                       \
        delete(ptr);        \
        ptr = NULL;         \
    }

#ifdef WIN32
#define GEOMARK_UINT64  __int64
#define GEOMARK_INT64   __int64
#else
#define GEOMARK_UINT64  unsigned long long
#define GEOMARK_INT64   long long
#endif
#endif
#endif
