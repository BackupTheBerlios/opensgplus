#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#ifndef _FLOOD2_H_
#define _FLOOD2_H_

/* INCLUDES */
#include <stdlib.h>

//#include <fstream.h>
#include <fstream>

#include "OSGWMcommontypes.h"
#include "OSGWMutil.h"
#include "OSGWMstarttri.h"
#include "OSGWMalginterface.h"

/* DEFINES */
#define FLOOD2_MAX_NUM_EMBED_BITS   65536 - 16
#define FLOOD2_MAX_NUM_EMBED_CHARS  8192 - 2

#define MAXPATH 256

/* MAIN PARAMETER STRUCT */
typedef struct
{
    char        WM_String[FLOOD2_MAX_NUM_EMBED_BITS];   /* watermark content in chars */

    int         bitString[FLOOD2_MAX_NUM_EMBED_BITS];   /* watermark content in bits*/
    int         bitStringLength;                        /* length of watermark */

    TriangleSet *mesh;                              /* pointer to mesh data */
    int         numBits;                            /* code numBits per height adjustment */

    int         actual_bit_id;                      /* used while traversing the tree */
    int         byte_boundary;                      /* counts bits from 0 to 7 */
    int         wait_for_term;                      /* for detecting lead in out */

    double      ratio[3];                           /* start-triangle edge length ratio */

    double      ael_st, maxDist, delta, DistTol;    /* MAIN parameters */
    double      MoveTo;                             /* used only temporary */

    char        wmFilename[MAXPATH];                /* watermark data (retrieving) */
    char        outFilename[MAXPATH];               /* wm-embedded model data (embedding) */

    double      maxDistScale;
    double      deltaScale;
    double      distToScale;

    TinyVector  startVertex;
    int         startTriangleVertex[3];

    FILE        *LineFile;

    /* pointers to arrays that hold vertex indices for
    colouring */
    /* if NULL, no vertex indices are written */
    GrowArray   *colouredVertices1;                 /* holds vertices constituting 
								  start triangle */
    GrowArray   *colouredVertices2;                 /* holds vertices "touched"
								  for encoding watermark bits */
} FLOOD2_WMParams;

/* preparing Lists for Vertices with same distance & vertices
   in tolerance area */
typedef struct  element
{
    struct element  *next;
    int             vertex_id;
    TinyVector      vertex;
    double          distance;
} ELEMENT;

/* preparing Binary Trees */
typedef struct  node
{
    double      distance;
    ELEMENT     *list;
    struct node *left;
    struct node *right;
    long        block_id;
} FLOOD2_NODE;

/* initializing struct for global statistics */
typedef struct
{
    int     max_possible;
    int     changed_vertices;
    double  accumulated_change;
    double  max_change;
} STAT;

//STAT statistics = {0,0,0,0};
/** DEFINITION of FUNCTIONS **/
int         flood2_embedWM(FLOOD2_WMParams *params);
int         flood2_retrieveWM(FLOOD2_WMParams *params);

/* flood calls this callback with it's outputs */
/* one of the two register functions MUST be called by the application
  prior to using flood2_embedWM() and flood2_retrieveWM */
int         flood2_registerMsgCallback(AlgExchangeData *data);
int         flood2_registerDefaults(void);

int         findStart(FLOOD2_WMParams *params);
TinyVector  CenterOfMass(int Triangle, TriangleSet *poly);
ELEMENT     *InsertInList(ELEMENT *list, TinyVector vertex, double distance,
                          int vertex_id, FLOOD2_WMParams *params);
FLOOD2_NODE *InsertInTree(FLOOD2_NODE *tree, TinyVector vertex, double distance,
                          int vertex_id, FLOOD2_WMParams *params, long block_id);
TinyVector  MoveVertex(TinyVector vertex, double new_distance,
                       FLOOD2_WMParams *params);
void        ShowParams(void);

void        accDistance_TraverseList(ELEMENT *list, int *entry_num,
                                     double *accu_dist);
void        MoveOut_TraverseList(ELEMENT *list, FLOOD2_WMParams *params);
void        Embed_TraverseTree(FLOOD2_NODE *tree, FLOOD2_WMParams *params);
int         Retrieve_TraverseTree(FLOOD2_NODE *tree, FLOOD2_WMParams *params);
#endif
#endif
