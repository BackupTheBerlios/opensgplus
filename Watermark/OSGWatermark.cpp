

//#ifdef OSG_STREAM_IN_STD_NAMESPACE
//#include <iostream>
//#include <fstream>
//#else
//#include <iostream.h>
//#include <fstream.h>
//#endif
//#include <GL/glut.h>
#include <OSGConfig.h>
#include <OSGSimpleGeometry.h>
//#include <OSGGLUTWindow.h>
#include <OSGSimpleSceneManager.h>
#include <OSGAction.h>

#include <OSGVRMLWriteAction.h>
#include <OSGVRMLFile.h>

#include <OSGSceneFileHandler.h>

#include <string>
#include <map>
#include <vector>

#include <OSGFieldContainerFactory.h>
#include <OSGSFSysTypes.h>
#include <OSGNode.h>
#include <OSGNodeCore.h>
#include <OSGThread.h>
#include <OSGAttachment.h>
#include <OSGMFVecTypes.h>
#include <OSGBaseFunctions.h>
#include <OSGChangeList.h>
#include <OSGFieldContainer.h>
#include <OSGGroup.h>

#include <OSGTriangleIterator.h>
#include <OSGFaceIterator.h>

#include <OSGGeometry.h>
#include <OSGPointLight.h>

#include <OSGSFFieldContainerPtr.h>
#include <OSGMFFieldContainerPtr.h>
#include <OSGSimpleGeometry.h>

#include <OSGMatrix.h>

#include <OSGAttachment.h>
#include <OSGSimpleAttachments.h>

#include "OSGWMflood2.h"
#include "OSGWMtriangleset.h"
#include "OSGWMcommontypes.h"

#include "OSGWMSceneGraphAccess.h"
#include "OSGWatermark.h"

OSG_USING_NAMESPACE

/*! OSGWatermark
	this class provides functions to embed a watermark string in a scene
	and to retrieve it.
	
	Watermarks are only embedded in Tris; higher level polygons are not
	used.
	The scene needs to contain at least one GeometryNode with enough
	triangles (dependant of the desired string length) to embed the watermark,
	or embedding/retrieving will fail.
*/
#define MAX_LENGTH_WM_DESCRIPTION   256
#define WATERMARK_TYPE_VFLOOD       2

typedef struct
{
    int     numVertices, numFaces;
    int     watermarkType;
    char    wmDescr[MAX_LENGTH_WM_DESCRIPTION];
} CollectedResults;

/* */
OSGWatermark::OSGWatermark(void)
{ }

/* */
OSGWatermark::~OSGWatermark(void)
{ }

/*! wrapper for the watermarking core algorithm  */
BOOL OSGWatermark::retrieveWM(NodePtr pScene, char *labelStr)
{
    int                 numCollectedResults;
    CollectedResults    *collectedResults;
    GEOMARK_BitArray    *isQuadVertex;
    int                 *vertexMap = NULL;
    TriangleSet         *quadFilteredTs = NULL;
    TriangleSet         *ts = NULL;
    int                 numQuadsFound = 0;
    BOOL                wmFound;
    double              bbDiameter;
    char                descrStr[MAX_LENGTH_WM_DESCRIPTION];
    int                 wmAlgorithm = WATERMARK_TYPE_VFLOOD;

    WmSceneGraphAccess  *wsa = new WmSceneGraphAccess(&pScene);

    collectedResults = (CollectedResults *) malloc(wsa->numMeshes_ * sizeof(CollectedResults));
    numCollectedResults = 0;

    int meshNum;
    for(meshNum = 0; meshNum < wsa->numMeshes_; meshNum++)
    {
        ts = new TriangleSet();

        wsa->convertToTs(ts, &numQuadsFound, &isQuadVertex, meshNum);

        /* our algorithms cannot handle quads, filter them out */
        if(numQuadsFound > 0)
        {
            wsa->filterOutQuads(ts, &quadFilteredTs, isQuadVertex, &vertexMap,
                                &bbDiameter);

            //printf("vertexMap=%d\n", vertexMap);
        }
        else
        {
            quadFilteredTs = ts;
        }

        if((numQuadsFound > 0) && (vertexMap == NULL))
        {
            SFATAL << "fatal error: could not convert quads!" << endl;
            return FALSE;
        }

        wmFound = FALSE;
        retrieveWMInternal(quadFilteredTs, &wmFound, descrStr, isQuadVertex);

        strcpy(collectedResults[numCollectedResults].wmDescr, "");
        if(wmFound)
        {
            strcpy(collectedResults[numCollectedResults].wmDescr, descrStr);
        }
        else
        {
            strcpy(collectedResults[numCollectedResults].wmDescr,
                   "retrieval failed");
        }

        if(isQuadVertex)
            GEOMARK_DELETE(isQuadVertex);

        if(numQuadsFound > 0)
        {
            wsa->propagateChanges(quadFilteredTs, ts, vertexMap);
        }

        collectedResults[numCollectedResults].numVertices = ts->vertices();
        collectedResults[numCollectedResults].numFaces = ts->triangles();
        collectedResults[numCollectedResults].watermarkType = wmAlgorithm;
        numCollectedResults++;

        delete ts;
    }

    BOOL    success = FALSE;
    char    result[MAX_LENGTH_WM_DESCRIPTION];
    strcpy(labelStr, "retrieval failed");

    int i = 0;

    while(!success && i < numCollectedResults)
    {
        SINFO <<
            "result #" <<
            (i + 1) <<
            ":\t" <<
            collectedResults[i].wmDescr <<
            endLog;
        if(strstr(collectedResults[i].wmDescr, "failed") == NULL)
        {
            success = TRUE;
            strncpy(labelStr, collectedResults[i].wmDescr,
                    MAX_LENGTH_WM_DESCRIPTION - 1);
        }

        i++;
    }

    GEOMARK_FREE(collectedResults);

    return success;
}

/*! wrapper for the watermarking core algorithm */
BOOL OSGWatermark::embedWM(NodePtr pScene, char *labelStr)
{
    int                 numCollectedResults;
    CollectedResults    *collectedResults;
    GEOMARK_BitArray    *isQuadVertex;
    int                 *vertexMap;
    TriangleSet         *quadFilteredTs;
    TriangleSet         *ts;
    int                 numQuadsFound;
    double              bbDiameter;
    BOOL                embedded;
    BOOL                oneEmbedded = FALSE;
    char                descrStr[MAX_LENGTH_WM_DESCRIPTION];
    int                 wmAlgorithm = WATERMARK_TYPE_VFLOOD;

    WmSceneGraphAccess  *wsa = new WmSceneGraphAccess(&pScene);

    collectedResults = (CollectedResults *) malloc(wsa->numMeshes_ * sizeof(CollectedResults));
    numCollectedResults = 0;

    int meshNum;

    for(meshNum = 0; meshNum < wsa->numMeshes_; meshNum++)
    {
        ts = new TriangleSet();

        wsa->convertToTs(ts, &numQuadsFound, &isQuadVertex, meshNum);

        /* our algorithms cannot handle quads, filter them out */
        if(numQuadsFound > 0)
        {
            wsa->filterOutQuads(ts, &quadFilteredTs, isQuadVertex, &vertexMap,
                                &bbDiameter);

            //printf("vertexMap=%d\n", vertexMap);
        }
        else
        {
            quadFilteredTs = ts;
        }

        if((numQuadsFound > 0) && (vertexMap == NULL))
        {
            SFATAL << "fatal error: could not convert quads!" << endl;
            return FALSE;
        }

        embedded = FALSE;
        embedWMInternal(quadFilteredTs, &embedded, labelStr, isQuadVertex);

        strcpy(collectedResults[numCollectedResults].wmDescr, "");
        if(embedded)
        {
            strcpy(collectedResults[numCollectedResults].wmDescr,
                   "watermark embedded");
            oneEmbedded = TRUE;
        }
        else
        {
            strcpy(collectedResults[numCollectedResults].wmDescr,
                   "embedding failed");
        }

        if(isQuadVertex)
            GEOMARK_DELETE(isQuadVertex);

        if(numQuadsFound > 0)
        {
            wsa->propagateChanges(quadFilteredTs, ts, vertexMap);
        }

        wsa->writeBack(ts, meshNum);

        collectedResults[numCollectedResults].numVertices = ts->vertices();
        collectedResults[numCollectedResults].numFaces = ts->triangles();
        collectedResults[numCollectedResults].watermarkType = wmAlgorithm;
        numCollectedResults++;

        delete ts;
    }

    delete wsa;

    return oneEmbedded; // tell if at least one WM has been embedded
}

/*! old Watermarking core function */
int OSGWatermark::embedWMInternal(TriangleSet *ts, BOOL *embedded,
                                  char *labelStr, GEOMARK_BitArray *isQuadVertex)
{
    FLOOD2_WMParams flood2Params;
    int             wmAlgReturnStatus;

    *embedded = FALSE;

    strcpy(flood2Params.WM_String, labelStr);
    flood2Params.deltaScale = 0.01;
    flood2Params.maxDistScale = 1000;
    flood2Params.numBits = 1;
    flood2Params.distToScale = flood2Params.deltaScale * ((1 << flood2Params.numBits) + 0.5);   /* needed ? */
    flood2Params.ratio[0] = 1.5;
    flood2Params.ratio[1] = 2;
    flood2Params.mesh = ts;
    flood2Params.colouredVertices1 = NULL;
    flood2Params.colouredVertices2 = NULL;

    wmAlgReturnStatus = flood2_embedWM(&flood2Params);
    if(wmAlgReturnStatus == 0)
    {   /* returnStatus==0 means success */
        *embedded = TRUE;
    }

    return 0;
}

/*! old Watermarking core function */
int OSGWatermark::retrieveWMInternal(TriangleSet *ts, BOOL *wmFound,
                                     char *descrStr,
                                     GEOMARK_BitArray *isQuadVertex)
{
    FLOOD2_WMParams flood2Params;
    int             wmAlgReturnStatus;
    int             i, l;

    *wmFound = FALSE;

    flood2Params.deltaScale = 0.01;
    flood2Params.maxDistScale = 1000;
    flood2Params.numBits = 1;
    flood2Params.distToScale = flood2Params.deltaScale * ((1 << flood2Params.numBits) + 0.5);   /* needed ? */
    flood2Params.ratio[0] = 1.5;
    flood2Params.ratio[1] = 2;
    flood2Params.mesh = ts;
    flood2Params.bitStringLength = 0;

    wmAlgReturnStatus = flood2_retrieveWM(&flood2Params);

    if((wmAlgReturnStatus != 0) || (flood2Params.bitStringLength == 0))
    {
        return 1;
    }
    else
    {
        /* shorten string to MAX_LENGTH_WM_DESCRIPTION */
        l = strlen(flood2Params.WM_String);
        l = MIN2(l, MAX_LENGTH_WM_DESCRIPTION - 2 - 1);
        strcpy(descrStr, "|");
        strncat(descrStr, flood2Params.WM_String, l);
        strcat(descrStr, "|");
        *wmFound = TRUE;
    }

    return 0;
}
