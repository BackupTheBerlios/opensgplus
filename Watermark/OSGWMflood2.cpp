#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#include "OSGWMalginterface.h"
#include "OSGWMmylimits.h"

#include "OSGWMflood2.h"

#define QUIET

#ifndef QUIET
#define DBG
#define PRINTF  printf
#else
#define DBG     / ## /
#define PRINTF  / ## /
#endif
STAT    statistics = { 0, 0, 0, 0 };

#define LOGCMD(x)                   gl_printMsgCb(REPORT_MSG_PRINT, x, -1)
#define LOGSTR                      gl_printStr
#define INCREASE_BAR(x)             gl_printMsgCb(REPORT_MSG_ADVANCE_BAR, NULL, x)
#define RESET_BAR()                 gl_printMsgCb(REPORT_MSG_RESET_BAR, NULL, -1)
#define TRAVERSE_MODE_WRITE_TEST    1               /* test writing, don't embed watermark */
#define TRAVERSE_MODE_WRITE         2               /* embed watermark */
#define TRAVERSE_MODE_READ          3               /* read watermark */

#define MAXPATH 255

#define CHANGE_RATIO

extern double   fixedRatios[NUM_FIXED_RATIOS][2];   /* in starttri.cpp */

#if 0

//extern CertStore allCerts;
extern CertStore    globalAllCerts;                 /* in certio.cpp */
#endif
typedef struct
{
    int     traverseMode;                           /* TRAVERSE_MODE_... */
    int     bitsProcessed;
    BOOL    traversalStopped;
    int     stopReason;
} FLOOD2_WMGlobalData;

/* forward decl */
//#ifdef FLOOD2_MAIN
int defaultMsgCallback(int msgType, char *str, int barIncr);

//#else
//extern int defaultMsgCallback(char*);
//#endif
static int (*gl_printMsgCb) (int, char *, int) = defaultMsgCallback;
static char gl_printStr[FLOOD2_MAX_NUM_EMBED_CHARS + 256];
static BOOL gl_operationAborted = FALSE;
static BOOL *gl_pOperationAborted = &gl_operationAborted;

float       gl_updateTreshold;
float       gl_updateCount;
int         gl_barIncrement;
int         gl_barCount;

/* message printing callback */
int flood2_registerMsgCallback(AlgExchangeData *data)
{
    gl_printMsgCb = data->cb;

    //getchar();
    //if (data->pOperationAborted!=NULL)
    gl_pOperationAborted = data->pOperationAborted;
    return 0;
}

/* */
int flood2_registerDefaults(void)
{
    gl_pOperationAborted = &gl_operationAborted;
    gl_printMsgCb = defaultMsgCallback;
    return 0;
}

//#ifdef FLOOD2_MAIN
int defaultMsgCallback(int mgType, char *str, int barIncr)
{
    /* just ignore */
    return 0;
}

//#endif
void    DeleteList(ELEMENT *list);
void    DeleteTree(FLOOD2_NODE *tree);

/*********************************************
     Show PARAMETER Handle 
*********************************************/
void ShowParams(void)
{
    printf("\nflood2 { e <wm-file> <mesh-in-file> <mesh-out-file> [<starttri-mark-file> <lines-mark-file>] | r <wm-file> <mesh-in-file> } \n\n");
    exit(1);
}

/********************************************************
   Traversing Tree and embedding bitstring
*********************************************************/
void Embed_TraverseTree(FLOOD2_NODE *tree, FLOOD2_WMParams *params)
{
    //char *bitblock; // all bits of one block are written here
    int i;
    int bitValue = 0;

    if(tree)
    {
        Embed_TraverseTree(tree->left, params);

        if(params->actual_bit_id < params->bitStringLength)
        {
            //bitblock = (char*) malloc ( sizeof(char) * params->numBits);
            // calculating new distance for vertices (with encoded bits)
            params->MoveTo =
                (
                    tree->block_id *
                    (2 + (1 << params->numBits)) *
                    params->delta
                ) +
                (1.5 * params->delta);

            for(i = params->numBits; (i > 0) &&
                    (params->actual_bit_id < params->bitStringLength);
            i--, params->actual_bit_id++)
            {
                if(params->bitString[params->actual_bit_id] == 0)
                {                   // more 1 than 0 expected ?
                    params->MoveTo += ((1 << (i - 1)) * params->delta);
                    bitValue += (1 << (i - 1));
                }
            }

            //actually moving the vertices
            MoveOut_TraverseList(tree->list, params);

            //printf("move out successful ?\n");
            //getchar();
        }
        else
            DeleteList(tree->list);

        Embed_TraverseTree(tree->right, params);

        GEOMARK_FREE(tree);
        tree = NULL;                // garbage collecting
    }                               /* if tree */
}

/********************************************************
   Traversing Tree and retrieving bitstring
*********************************************************/
int Retrieve_TraverseTree(FLOOD2_NODE *tree, FLOOD2_WMParams *params)
{
    double  enc, distance, acc_dist = 0;
    int     i, j, k, entry_num = 0;
    char    bit[256];               // all the bits of one block
    int     rv;
    int     bitValue;
    int     bitMask;

    rv = -1;
    if(tree)
    {
        if(tree->left != NULL)
        {
            rv = Retrieve_TraverseTree(tree->left, params);
        }
        else
            rv = 0;

        if(!rv)
        {
            if(params->wait_for_term < 8)
            {                       // wait_for_term gets 8 when lead-out is found
                //bit = (char*) malloc(params->numBits*sizeof(char));
                // calculating the average distance
                accDistance_TraverseList(tree->list, &entry_num, &acc_dist);
                distance = acc_dist / entry_num;

                enc = distance - (double)
                    (
                        tree->block_id *
                        (2.0 + (1 << params->numBits)) *
                        params->delta
                    );

                // calculating the bit block from the distance of the vertices
                j = params->numBits;

                if((enc < params->delta) ||
                   (enc > (params->delta * (1 + (1 << params->numBits)))))
                {
                    // PRINTF("bit-block lost!\n");
                    for(i = 0; i < params->numBits; i++)
                        bit[i] = -1;
                }
                else
                {
                    bitValue = (int) floor((enc - params->delta) / params->delta);
                    bitMask = 1 << (params->numBits - 1);
                    for(i = 0; i < params->numBits; i++)
                    {
                        bit[i] = ((bitValue & bitMask) == 0);
                        bitMask >>= 1;
                    }
                }

                // adding the bitblock the the entire wm_bit_string and checking for lead-in & lead-out
                for(i = 0; (i < params->numBits) && (params->wait_for_term < 8);
                    i++, params->actual_bit_id++)
                {
                    if(bit[i] == 1)
                    {
                        params->bitString[params->actual_bit_id] = 1;

                        /* BUGFIX */
                        /* lead in starts at byte boundary */
                        if(params->wait_for_term == 0)
                        {
                            if(params->byte_boundary == 0)
                                params->wait_for_term++;
                        }
                        else if(params->wait_for_term < -4 ||
                                params->wait_for_term >= 0)
                        {
                            params->wait_for_term++;
                        }
                        else
                        {
                            //                       fprintf(stdout, "No Watermark Lead-In found or Watermark destroyed!\n");
                            //exit(1);
                            //DeleteTree(tree);
                            return -1;
                        }

                        if(params->wait_for_term > 7)
                        {
                            params->bitStringLength = params->actual_bit_id + 1;
                        }
                    }
                    else if(bit[i] == 0)
                    {
                        params->bitString[params->actual_bit_id] = 0;
                        if(params->wait_for_term > -5 &&
                           params->wait_for_term < 0)
                        {
                            params->wait_for_term++;
                        }
                        else if(params->wait_for_term >= 0)
                        {
                            params->wait_for_term = 0;
                        }
                        else
                        {
                            //                     fprintf(stdout, "No Watermark Lead-In found or Watermark destroyed!\n");
                            //exit(1);
                            //DeleteTree(tree);
                            return -1;
                        }
                    }
                    else
                    {
                        //printf("bit[%d]=%d\n",i,bit[i]);
                        //				  fprintf(stdout, "No Watermark Lead-Out found or Watermark destroyed!\n");
                        //exit(1);
                        //DeleteTree(tree);
                        return -1;
                    }

                    if(params->byte_boundary == 7)
                    {
                        params->byte_boundary = 0;
                    }
                    else
                    {
                        params->byte_boundary++;
                    }
                }                   /* for */
            }

            if(tree->right != NULL)
            {
                rv = Retrieve_TraverseTree(tree->right, params);
            }
            else
                rv = 0;
        }                           /* if !rv */

        //ee(tree);
    }                               /* if tree */

    return rv;
}

/************************************************************* 
   Traversing the list and calculating the accumulated distance
**************************************************************/
void accDistance_TraverseList(ELEMENT *list, int *entry_num, double *accu_dist)
{
    if(list)
    {
        *accu_dist += list->distance;
        (*entry_num)++;
        accDistance_TraverseList(list->next, entry_num, accu_dist);

        //GEOMARK_FREE(list);list)=NULL; // garbage collecting
    }
}

/********************************************************
   Traversing the list and moving out all the vertices
   to params->MoveTo
*********************************************************/
void MoveOut_TraverseList(ELEMENT *list, FLOOD2_WMParams *params)
{
    if(list)
    {
        params->mesh->setVertex(list->vertex_id,
                                MoveVertex(list->vertex, params->MoveTo, params));

        /* color vertex (second color list) */
        if(params->colouredVertices2)
        {
            params->colouredVertices2->appendElement(&list->vertex_id);
        }

        MoveOut_TraverseList(list->next, params);

        //cout << list->vertex;
        GEOMARK_FREE(list);
        list = NULL;                // garbage collecting
    }
}

/****************************
     MoveVertex
****************************/
TinyVector MoveVertex(TinyVector vertex, double new_distance,
                      FLOOD2_WMParams *params)
{
    TinyVector  p1, new_vertex;
    double      old_distance;

    p1 = vertex - params->startVertex;

    old_distance = tv_abs(p1);
    p1.normalize();

    new_vertex = vertex + (p1 * (new_distance - old_distance));

#ifdef FLOOD2_MAIN
    // marking the lines in flood2-markfile
    fprintf(params->LineFile, "%f %f %f\n%f %f %f\n", new_vertex[0],
            new_vertex[1], new_vertex[2], params->startVertex[0],
            params->startVertex[1], params->startVertex[2]);
#endif

    // making the statistics stuff
    statistics.changed_vertices++;
    if(statistics.max_change < (tv_abs(vertex - new_vertex)))
    {
        statistics.max_change = tv_abs(vertex - new_vertex);
    }

    statistics.accumulated_change += tv_abs(vertex - new_vertex);

    return new_vertex;
}

/***************************
   Insert Vertex in List
****************************/
ELEMENT *InsertInList(ELEMENT *list, TinyVector vertex, double distance,
                      int vertex_id, FLOOD2_WMParams *params)
{
    if(!list)
    {
        list = (ELEMENT *) malloc(sizeof(ELEMENT));
        list->vertex_id = vertex_id;
        list->vertex = vertex;
        list->distance = distance;
        list->next = NULL;
    }
    else
    {
        list->next = InsertInList(list->next, vertex, distance, vertex_id,
                                  params);
    }

    return list;
}

/* */
void DeleteList(ELEMENT *list)
{
    if(list != NULL)
    {
        if(list->next != NULL)
            DeleteList(list->next);
        GEOMARK_FREE(list);
        list = NULL;
    }
}

/****************************
   Insert Vertex in Tree 
*****************************/
FLOOD2_NODE *InsertInTree(FLOOD2_NODE *tree, TinyVector vertex, double distance,
                          int vertex_id, FLOOD2_WMParams *params, long block_id)
{
    double  i;

    if(!tree)
    {
        tree = (FLOOD2_NODE *) malloc(sizeof(FLOOD2_NODE));
        tree->left = NULL;
        tree->right = NULL;
        tree->list = NULL;

        tree->distance = distance;
        tree->block_id = block_id;

        tree->list = InsertInList(tree->list, vertex, distance, vertex_id,
                                  params);
        statistics.max_possible++;
    }
    else
    {
        if(tree->block_id < block_id)
        {
            tree->right = InsertInTree(tree->right, vertex, distance,
                                       vertex_id, params, block_id);
        }
        else if(tree->block_id > block_id)
        {
            tree->left = InsertInTree(tree->left, vertex, distance, vertex_id,
                                      params, block_id);
        }
        else
        {
            InsertInList(tree->list, vertex, distance, vertex_id, params);
        }
    }

    return tree;
}

/* */
void DeleteTree(FLOOD2_NODE *tree)
{
    if(tree != NULL)
    {
        if(tree->left != NULL)
            DeleteTree(tree->left);
        if(tree->right != NULL)
            DeleteTree(tree->right);
        if(tree->list != NULL)
            DeleteList(tree->list);
        GEOMARK_FREE(tree);
        tree = NULL;
    }
}

/* */
void DumpTree(FLOOD2_NODE *tree)
{
    if(tree != NULL)
    {
        //PRINTF("*traversing %d\n", tree->block_id);
        DumpTree(tree->left);
        DumpTree(tree->right);
    }
}

#if 0
**************************search StartTriangle ************************** /
    int findStart (FLOOD2_WMParams * params)
    {
        SortEntry   *matches;
    int         numMatches, numRemoved;
    int         i;

    // max. possible starttriangles = all / 10
    matches = (SortEntry *) malloc(sizeof(SortEntry) * params->mesh->vertices() / 10);

    scanTrianglesForEdgeRatio
        (
            params->ratio, params->mesh, matches, &numMatches, params->mesh->
                triangles(), 0.05, FALSE
        );

    // Writing the Vertices of the StartTriangle
    for(i = 0; i < 3; i++)
    {
        params->startTriangleVertex[i] = params->mesh->getTriangleVertex(matches[0].gp1, i);
    }
    params->ael_st = 2;             //matches[0].gp2;
    return matches[0].gp1;
}
#endif

/***************************
   calc Center of Mass
****************************/
TinyVector CenterOfMass(int Triangle, TriangleSet *poly)
{
    int         i;

    TinyVector  p1, p2, p3, center;

    p1 = poly->getVertex(poly->getTriangleVertex(Triangle, 0));
    p2 = poly->getVertex(poly->getTriangleVertex(Triangle, 1));
    p3 = poly->getVertex(poly->getTriangleVertex(Triangle, 2));

    for(i = 0; i < 3; i++)
    {
        center[i] = (p1[i] + p2[i] + p3[i]) / 3;
    }

    return center;
}

/*********************************
  FLOOD EMBED SUBMAIN
**********************************/
int flood2_embedWM(FLOOD2_WMParams *params)
{
    int                 n, i, j, k, len;
    FLOOD2_NODE         *Tree_root = NULL;
    ELEMENT             *List_root = NULL;

    TinyVector          Vertex;
    double              distance;
    long                block_id;

    FLOOD2_WMGlobalData gld;

    double              meanDistance, quantilValue;
    double              ratio[3];
    SortEntry           *matches;
    int                 numMatches;
    double              tolDiff;
    int                 numRemoved;
    BOOL                startTriangleOK;
    int                 startTriangle;
    TinyVector          resultVectors[3];
    double              lengths[3];
    FILE                *startTriFile;

    /** Preparing real bitString from old charString (bitString)**/
    matches = (SortEntry *) malloc(sizeof(SortEntry) * MAX_NUM_FACES);

    len = strlen(params->WM_String);
    params->bitStringLength = len * 8;

    //bzero(params->bitString, params->bitStringLength);
    n = 8;
    for(i = 0; i < len; i++)
    {
        for(j = 7; j >= 0; j--)
        {
            if((params->WM_String[i] & (1 << j)) > 0)
                params->bitString[n] = 1;
            else
                params->bitString[n] = 0;
            n++;
        }
    }

    /* adding lead-in and lead-out */
    for(i = 0; i < 4; i++)
    {
        params->bitString[i] = 1;
        params->bitString[8 + params->bitStringLength + i] = 1;
        params->bitString[4 + i] = 0;
        params->bitString[12 + params->bitStringLength + i] = 1;
    }

    params->bitStringLength += 16;  /* add lead-in/out length */

    //params->bitString[params->bitStringLength]='\0';
    /** bitString now prepared **/
    LOGCMD("\n################# starting flood2 embedding process ################\n");
    LOGCMD("scanning for triangles matching ratio\n");

    /* step 1: choose start triangle matching user defined ratios 
             and the averaged edge lengths close to 25% quantil
  */
    // get mean edge length of all vertices in mesh and
    // quantil value of mean edge length
    calcVertexDist(params->mesh, &meanDistance, 0.25, &quantilValue);

    ratio[0] = 1;
    ratio[1] = params->ratio[0];
    ratio[2] = params->ratio[1];

#if 0
    EdgeRatioResult *edgeRatioResult = NULL;
    allocEdgeRatioResultArray(&edgeRatioResult, params->mesh->triangles());

    scanTrianglesForFixedEdgeRatios(params->mesh, params->mesh->triangles(),
                                    0.05, edgeRatioResult, quantilValue, TRUE);

    freeEdgeRatioResultArray(edgeRatioResult);

    exit(0);
#endif

    // Find triangles for which the difference to the optimal ratio
    // is below the given threshold (0.05). Result is contained in
    // "matches" (array holding SortEntry items sorted by ascending
    // difference).
    scanTrianglesForEdgeRatio(ratio, params->mesh, matches, &numMatches,
                              params->mesh->triangles(), 0.05, FALSE);

    //sprintf(LOGSTR,"%d matching triangles\n",numMatches);
    //LOGCMD(LOGSTR);
    /* 
  Remove matches whose average lengths > 50% quantil.

  In contrast: When retriving the wm, start triangles are selected 
  based on their ratios only.
  */
    tolDiff = 2 * quantilValue;

    numRemoved = 0;
    for(i = 0; i < numMatches; i++)
    {
        if(matches[i].gp2 > tolDiff)
        {
            numRemoved++;
            matches[i].gp1 = -1;    /* Mark this one as removed */
        }
    }

    if(numRemoved > 0)
    {
        sprintf(LOGSTR, "%d removed because of avg edge lengths\n", numRemoved);
        LOGCMD(LOGSTR);
    }

    /* just print out matches */
    /* 
  PRINTF("** matches to test\n");
  if (numMatches-numRemoved > 0)
  {
    for (i=0;i<numMatches;i++)
      if (matches[i].gp1 != -1)
      {
        PRINTF("%d,",matches[i].gp1);
      }
  }
  PRINTF("\n");
*/
    List_root = NULL;
    Tree_root = NULL;

    /* step 2: choose a start triangle with sufficient capacity
  */
    startTriangleOK = FALSE;

    if(numMatches - numRemoved > 0)
    {
        j = 0;

        sprintf(LOGSTR,
                "testing candidates for required %d bits of capacity\n", params->bitStringLength);
        LOGCMD(LOGSTR);

        gl_updateTreshold = numMatches * 0.05;
        gl_updateCount = 0.0;
        gl_barIncrement = 5;
        gl_barCount = 0;

        while
        (
            (!startTriangleOK) &&
            (j < numMatches) &&
            (!(*gl_pOperationAborted))
        )
        {
            /* update progress of every 5% */
            if(matches[j].gp1 != -1)
            {
                startTriangle = matches[j].gp1;

                for(k = 0; k < 3; k++)
                {
                    params->startTriangleVertex[k] = params->mesh->getTriangleVertex(startTriangle, k);
                }

                //params->startVertex=
                //	triangleCenter(startTriangle,params->mesh);
                changeRatio(ratio, params->mesh, startTriangle,
                            CHANGE_RATIO_KEEP_COM_CONSTANT, TRUE, resultVectors);

                //for (i=0;i<3;i++)
                //{
                //  printf("CHECK*:\n");
                //  params->mesh->setVertex(params->startTriangleVertex[i],
                //    resultVectors[i]);
                //  cout << resultVectors[i] << "\n";
                //}
                //for (k=0;k<3;k++)
                //  cout << "resultVectors(" << k << ")=" << resultVectors[k] << "\n";
                triangleEdgeLengths(resultVectors[0], resultVectors[1],
                                    resultVectors[2], lengths);

                params->startVertex =
                    (
                        resultVectors[0] +
                        resultVectors[1] +
                        resultVectors[2]
                    ) /
                    3;

                params->ael_st = (lengths[0] + lengths[1] + lengths[2]) / 3.;
                params->maxDist = params->maxDistScale * params->ael_st;
                params->delta = params->deltaScale * params->ael_st;
                params->DistTol = params->distToScale * params->ael_st;

                //printf("maxDistScale=%f\n",params->maxDistScale);
                //printf("deltaScale=%f\n",params->deltaScale);
                //printf("distToScale=%f\n",params->distToScale);	
                //exit(0);
                /** Checking all vertices for distance and writing them to trees / lists **/
                /*    for (i=0; i<3; i++)
				printf(":%d\n", params->startTriangleVertex[i]);
		*/
                List_root = NULL;
                Tree_root = NULL;
                statistics.max_possible = 0;
                statistics.changed_vertices = 0;
                statistics.accumulated_change = 0;
                statistics.max_change = 0;

                for(i = 0; i < (int) params->mesh->vertices(); i++)
                {
                    if(i != params->startTriangleVertex[0])
                    {
                        if(i != params->startTriangleVertex[1])
                        {
                            if(i != params->startTriangleVertex[2])
                            {
                                distance = (double) tv_abs(params->startVertex -
                                                               (Vertex = params->mesh->getVertex(i)));
                                block_id = (long)
                                    (
                                        distance /
                                            (
                                                params->delta *
                                                    (2 + (1 << params->numBits))
                                            )
                                    );

                                if(distance <
                                       (params->maxDist - params->DistTol))
                                {
                                    Tree_root = InsertInTree(Tree_root, Vertex,
                                                             distance, i,
                                                             params, block_id);
                                }
                                else if(distance <
                                            (params->maxDist + params->DistTol))
                                {
                                    List_root = InsertInList(List_root, Vertex,
                                                             distance, i,
                                                             params);
                                }
                            }
                        }
                    }
                }

                /** all relevant vertices are now written **/
                //DumpTree(Tree_root);
                /** check if string could be completely encoded **/
                if((
                       statistics.max_possible *
                   params->numBits
                       ) < params->bitStringLength)
                {
                    //		fprintf(stdout, "Bit-String too long to embed with these options!\n");
                    //		fprintf(stdout, "Maximum is %d bits !\n",statistics.max_possible*params->numBits);
                    //exit(1);
                    /* delete tree and (outsider-)list */
                    //printf("segmentation fault ?\n");
                    //getchar();
                    DeleteTree(Tree_root);
                    DeleteList(List_root);

                    //Tree_root=NULL;
                    //List_root=NULL;
                    //printf("no\n");
                    //DumpTree(Tree_root);
                    //LOGCMD(".");
                }
                else
                {
                    startTriangleOK = TRUE;
                    LOGCMD("+");
                }

                gl_updateCount++;
                if(gl_updateCount >= gl_updateTreshold)
                {
                    float   times;
                    int     addIncrement;

                    times = gl_updateCount / gl_updateTreshold;
                    addIncrement = (int) (gl_barIncrement * times);
                    gl_barCount += addIncrement;
                    INCREASE_BAR(addIncrement);
                    gl_updateCount = gl_updateCount - (times * gl_updateTreshold);

                    //printf("UPDATE-COUNTER: %d,%f\n",times,gl_updateCount);
                }

                //else if ((j==numMatches-1) && (gl_barCount < 100)) {
                //  INCREASE_BAR(100-gl_barCount);
                //}
            }                       /* matches[j].gp1 !=- 1 */

            j++;
        }                           /* while ((!startTriangleOK) && (j<numMatches)) */

        LOGCMD("\n");

        if(gl_barCount < 100)
        {
            INCREASE_BAR(100 - gl_barCount);
        }
    }                               /* if numMatches-numRemoved > 0 */

    RESET_BAR();

    if(*gl_pOperationAborted)
        startTriangleOK = FALSE;    /* prevent mesh from bein altered */

    if(startTriangleOK)
    {
        /* write out start triangle file */
#ifdef FLOOD2_MAIN
        startTriFile = fopen("./start.dat", "w");
        fprintf(startTriFile, "1\n%d\n", startTriangle);
        fclose(startTriFile);
#endif

        /* now modify the start triangle */
        for(i = 0; i < 3; i++)
        {
            //PRINTF("CHECK:\n");
            params->mesh->setVertex(params->startTriangleVertex[i],
                                    resultVectors[i]);
#ifndef QUIET
            cout << resultVectors[i] << "\n";
#endif
        }

        /* add start triangle vertex indices to color list 1 */
        if(params->colouredVertices1)
        {
            for(i = 0; i < 3; i++)
            {
                params->colouredVertices1->appendElement(&params->startTriangleVertex[i]);
            }
        }

        /* we do not ditinguish between vertices moved out of
	  search range and vertices moved in order to embed
	  watermark bits -> possibly changes this */
        /* coloring with second color is done in MoveOut */
        LOGCMD("embedding the watermark now\n");

        /** moving all vertices out of the tolerance area **  direction: only out */
        params->MoveTo = params->maxDist + params->DistTol;
        MoveOut_TraverseList(List_root, params);

        //printf("moved out vertices\n");
        //getchar();
        /** embedding bit string  **/
        params->actual_bit_id = 0;
        params->byte_boundary = 0;

        Embed_TraverseTree(Tree_root, params);

        //getchar();
#ifdef FLOOD2_MAIN
        // Writing the new 3D figure to file
        if(1)
        {
            ofstream    data2(params->outFilename);
            params->mesh->exportOBJ(data2);
            data2.close();
        }
#endif
        sprintf(LOGSTR,
                "*** statistics ***\n%d vertices modified for coding wm bits\n%d bits was the maximum capacity\n%f was the maximum change\n%f was the accumulated changes\n", statistics.changed_vertices, statistics.max_possible,
                statistics.max_change, statistics.accumulated_change);
        LOGCMD(LOGSTR);
    }

    //getchar();
    GEOMARK_FREE(matches);
    matches = NULL;

    if(startTriangleOK)
        return 0;
    else
        return -1;
}

/****************************
    FLOOD RETRIEVE SUBMAIN
****************************/
int flood2_retrieveWM(FLOOD2_WMParams *params)
{
    int         n, i, j, k, len;
    FLOOD2_NODE *Tree_root = NULL;

    //    ELEMENT *List_root = NULL;
    TinyVector  Vertex;
    double      distance;
    long        block_id;

    double      meanDistance, quantilValue;
    double      ratio[3];
    SortEntry   *matches;
    int         numMatches;
    double      tolDiff;
    int         numRemoved;
    BOOL        watermarkFound;
    int         startTriangle;
    TinyVector  resultVectors[3];
    double      lengths[3];
    FILE        *startTriFile;
    int         rv;

    matches = (SortEntry *) malloc(sizeof(SortEntry) * MAX_NUM_FACES);

    LOGCMD("\n################# starting flood2 retrieving process ################\n");
    LOGCMD("scanning for triangles matching ratio\n");

    /* step 1: choose start triangle matching user defined ratios 
             and the averaged edge lengths close to 25% quantil
  */
    calcVertexDist(params->mesh, &meanDistance, 0.25, &quantilValue);

    ratio[0] = 1;
    ratio[1] = params->ratio[0];
    ratio[2] = params->ratio[1];

    scanTrianglesForEdgeRatio(ratio, params->mesh, matches, &numMatches,
                              params->mesh->triangles(), 0.05, FALSE);

    sprintf(LOGSTR, "%d matching triangles\n", numMatches);
    LOGCMD(LOGSTR);

    /* remove matches whose average edge lengths differs by more than 50% from
    25% quantil */
    /* CHANGED: remove matches whose average lengths > 40% (25% in embedding !) quantil */
    /* in contrast: when retriving the wm,
    start triangles are selected based on their ratios only */
    tolDiff = 2 * quantilValue;

    numRemoved = 0;
#if 1
    for(i = 0; i < numMatches; i++)
    {
        //if (fabs(matches[i].gp2-quantilValue) > tolDiff)
        if(matches[i].gp2 > tolDiff)
        {
            numRemoved++;
            matches[i].gp1 = -1;    /* "remove" match */
        }
    }
#endif
    if(numRemoved > 0)
    {
        sprintf(LOGSTR, "%d removed because of avg edge lengths\n", numRemoved);
        LOGCMD(LOGSTR);
    }

    /* just print out matches */
    /*
  PRINTF("** matches to test\n");
  if (numMatches-numRemoved > 0)
  {
    for (i=0;i<numMatches;i++)
      if (matches[i].gp1 != -1)
      {
        PRINTF("%d,",matches[i].gp1);
      }
  }
  PRINTF("\n");
  */
    //exit(0);
    /* step 2: choose a start triangle with sufficient capacity
  */
    if(numMatches - numRemoved > 0)
    {
        j = 0;
        watermarkFound = FALSE;

        sprintf(LOGSTR, "testing candidates\n");
        LOGCMD(LOGSTR);

        gl_updateTreshold = numMatches * 0.05;
        gl_updateCount = 0.0;
        gl_barIncrement = 5;
        gl_barCount = 0;

        while
        (
            (!watermarkFound) &&
            (j < numMatches) &&
            (!(*gl_pOperationAborted))
        )
        {
            if(matches[j].gp1 != -1)
            {
                startTriangle = matches[j].gp1;

                //startTriangle=8796;
                for(k = 0; k < 3; k++)
                {
                    params->startTriangleVertex[k] = params->mesh->getTriangleVertex(startTriangle, k);
                    resultVectors[k] = params->mesh->getVertex(params->startTriangleVertex[k]);
                }

                params->startVertex =
                    (
                        resultVectors[0] +
                        resultVectors[1] +
                        resultVectors[2]
                    ) /
                    3;

                //changeRatio(ratio,params->mesh,startTriangle,
                //  CHANGE_RATIO_KEEP_COM_CONSTANT,TRUE,resultVectors);
                //for (k=0;k<3;k++)
                //  cout << "resultVectors(" << k << ")=" << resultVectors[k] << "\n";
                triangleEdgeLengths(startTriangle, params->mesh, lengths);

                params->ael_st = (lengths[0] + lengths[1] + lengths[2]) / 3.;
                params->maxDist = params->maxDistScale * params->ael_st;
                params->delta = params->deltaScale * params->ael_st;
                params->DistTol = params->distToScale * params->ael_st;

                /** Checking all vertices for distance and writing them to trees / lists **/
                /*    for (i=0; i<3; i++)
        printf(":%d\n", params->startTriangleVertex[i]);
*/
                for(i = 0; i < (int) params->mesh->vertices(); i++)
                {
                    if(i != params->startTriangleVertex[0])
                    {
                        if(i != params->startTriangleVertex[1])
                        {
                            if(i != params->startTriangleVertex[2])
                            {
                                distance = (double) tv_abs(params->startVertex -
                                                               (Vertex = params->mesh->getVertex(i)));
                                block_id = (long)
                                    (
                                        distance /
                                            (
                                                params->delta *
                                                    (2 + (1 << params->numBits))
                                            )
                                    );

                                //PRINTF("%d) delta: %f | blockID: %d | distance: %f\n", i,params->delta, block_id, distance);
                                if(distance <
                                       (params->maxDist - params->DistTol))
                                {
                                    //PRINTF("insert %d,%d\n",i,block_id);
                                    Tree_root = InsertInTree(Tree_root, Vertex,
                                                             distance, i,
                                                             params, block_id);
                                }
                            }
                        }
                    }
                }

                /** all relevant vertices are now written **/
                /** retrieving bit string  **/
                params->bitStringLength = 0;
                params->wait_for_term = -8;
                params->actual_bit_id = 0;
                params->byte_boundary = 0;

                //DumpTree(Tree_root);
                rv = Retrieve_TraverseTree(Tree_root, params);

                //DumpTree(Tree_root);
                DeleteTree(Tree_root);
                Tree_root = NULL;   /* important ! */

                //exit(0);
                if(rv == -1)
                {
                    /* */
                }
                else if(params->bitStringLength % 8)
                {
                    //        fprintf(stdout, "bitString has wrong length: %d\n", params->bitStringLength);
                    /* tree should have been be deallocated  in call
          Retrieve_TraverseTree() */
                }
                else
                {
                    watermarkFound = TRUE;
                }

                len = (params->bitStringLength / 8) - 2;

                //	fprintf(stdout,"bitstringlen: %d  len: %d\n", params->bitStringLength, len);
                if(watermarkFound)
                {
                    params->bitStringLength -= 8;
                    n = 8;
                    for(i = 0; i < len; i++)
                    {
                        params->WM_String[i] = 0;
                        for(j = 7; j >= 0; j--)
                        {
                            if(params->bitString[n] == 1)
                            {
                                params->WM_String[i] |= (1 << j);
                            }
                            else
                            { }

                            n++;
                        }
                    }

                    params->WM_String[len] = '\0';

                    sprintf(LOGSTR,
                            "\n######### watermark found: length=%d bits ###########\n",
                        params->bitStringLength);
                    LOGCMD(LOGSTR);

                    sprintf(LOGSTR, ">%s<\n", params->WM_String);
                    LOGCMD(LOGSTR);
                }                   /* if (watermarkFound) */
                else
                {
                    //LOGCMD(".");
                }

                gl_updateCount++;
                if(gl_updateCount >= gl_updateTreshold)
                {
                    float   times;
                    int     addIncrement;

                    times = gl_updateCount / gl_updateTreshold;
                    addIncrement = (int) (gl_barIncrement * times);
                    gl_barCount += addIncrement;
                    INCREASE_BAR(addIncrement);
                    gl_updateCount = gl_updateCount - (times * gl_updateTreshold);

                    //printf("UPDATE-COUNTER: %d,%f\n",times,gl_updateCount);
                }

                //else if ((j==numMatches-1) && (gl_barCount < 100)) {
                //  INCREASE_BAR(100-gl_barCount);
                //}
            }                       /* matches[j].gp1 !=- 1 */

            j++;
        }                           /* while ((!startTriangleOK) && (j<numMatches)) */

        if(gl_barCount < 100)
        {
            INCREASE_BAR(100 - gl_barCount);
        }
    }                               /* if numMatches-numRemoved > 0 */

    RESET_BAR();

    GEOMARK_FREE(matches);
    matches = NULL;
    return 0;
}
#endif
