#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#ifndef _MYLIMITS_H_
#define _MYLIMITS_H_

/* limits: change these if needed and recompile */
/* mesh size */
#define MAX_NUM_VERTICES    400000
#define MAX_NUM_FACES       400000

/* geomark */
#define MAX_NUM_BINS            1000
#define MAX_NUM_RESTRICTIONS    256

/* for nurbs */
#define MAX_NUM_NURBS_SURFACES  500

/* for certificates */
#define MAX_NUM_CERTIFICATES    32
#define MAX_NUM_TRUSTED_CERTS   16
#define MAX_NUM_USER_CERTS      16

/* limits for numerical stability */
/* absolute values, maybe switch to fractions of bounding box */
/* sum of sqared coordinate diffs, no square root */
#define ZERO_TOL_EDGE_LENGTH    ((double) 1e-10)

/* sqare root of sum of squared coordinate dffs */
#define ZERO_TOL_VEC_LENGTH ((double) 1e-10)

/* zero triangle face size */
#define ZERO_TOL_FACE_SIZE  ((double) 1e-10)
#endif
#endif
