#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
#include <stdio.h>

//#include <fstream.h>
#include <fstream>
#include <iostream>
#include <iomanip>

//#include <iostream.h>
//#include <iomanip.h> /* for setprecision() */
#include "OSGWMmylimits.h"
#include "OSGWMtriangleset.h"

#include "OSGWMutil.h"

#ifdef COLDET_PRESENT
#include "spatsort.h"
#endif
#ifndef WIN32
#ifndef BOOL
#define BOOL    int
#define TRUE    1
#define FALSE   0
#define MAXPATH 255
#endif
#endif
extern int          global_debug_hashtab;

static const double ZERO_TOL = 1e-06;

/* */

int compareFacesByVertexIndex(const void *e1, const void *e2)
{
    if(((SortTrianglesByVertexIndicesEntry *) e1)->vertexId <
           ((SortTrianglesByVertexIndicesEntry *) e2)->vertexId)
        return -1;
    else if(((SortTrianglesByVertexIndicesEntry *) e1)->vertexId ==
                ((SortTrianglesByVertexIndicesEntry *) e2)->vertexId)
        return 0;
    else
        return 1;
}

/* */
int compareEdgesFirstVertexThenSecond(const void *e1, const void *e2)
{
    GEOMARK_Edge    a1, a2;

    a1 = *((GEOMARK_Edge *) e1);
    a2 = *((GEOMARK_Edge *) e2);
    if(a1.ind[0] < a2.ind[0])
        return -1;
    else if(a1.ind[0] > a2.ind[0])
        return 1;
    else
    {
        if(a1.ind[1] < a2.ind[1])
            return -1;
        else if(a1.ind[1] > a2.ind[1])
            return 1;
        else
            return 0;
    }
}

/* */
int compareEdgesSecondVertexThenFirst(const void *e1, const void *e2)
{
    GEOMARK_Edge    a1, a2;

    a1 = *((GEOMARK_Edge *) e1);
    a2 = *((GEOMARK_Edge *) e2);
    if(a1.ind[1] < a2.ind[1])
        return -1;
    else if(a1.ind[1] > a2.ind[1])
        return 1;
    else
    {
        if(a1.ind[0] < a2.ind[0])
            return -1;
        else if(a1.ind[0] > a2.ind[0])
            return 1;
        else
            return 0;
    }
}

/* */
int compareFacesByComZ(const void *e1, const void *e2)
{
    if(((SortTrianglesByComZ *) e1)->comZ < ((SortTrianglesByComZ *) e2)->comZ)
        return -1;
    else if(((SortTrianglesByComZ *) e1)->comZ ==
                ((SortTrianglesByComZ *) e2)->comZ)
        return 0;
    else
        return 1;
}

/* */
int compareAngles(const void *e1, const void *e2)
{
    if(((SortAngles *) e1)->angle < ((SortAngles *) e2)->angle)
        return -1;
    else if(((SortAngles *) e1)->angle == ((SortAngles *) e2)->angle)
        return 0;
    else
        return 1;
}

/* */
void TriangleSet::sortTriangles_(void)
{
    int i, j;

    for(i = 0; i < nf_; i++)
    {
        for(j = 0; j < 3; j++)
        {
            trianglesSorted_[j][i].triangleId = i;
            trianglesSorted_[j][i].vertexId = faces_[i].ind[j];
        }
    }

    for(j = 0; j < 3; j++)
    {
        qsort(trianglesSorted_[j], nf_,
              sizeof(SortTrianglesByVertexIndicesEntry),
              compareFacesByVertexIndex);
    }
}

/* */
void TriangleSet::generateAdjList_(void)
{
    int                 i, j, n;
    int                 l, index;
    AdjTrianglesIndex   adjTrIndex;

    sortTriangles_();

    index = 0;

    for(i = 0; i < np_; i++)
    {
        getAdjTriangles_(i, tmpTriangleList_[1]);
        n = tmpTriangleList_[1]->getNumElements();

        adjTrIndex.startIndex = index;
        adjTrIndex.lastIndex = index + n - 1;

        /* lastIndex < startIndex for 0 elements ! */
        adjTrianglesIndex_->appendElement(&adjTrIndex);
        if(n > 0)
        {
            adjTriangles_->copy(tmpTriangleList_[1], FALSE);
        }

        index += n;
        tmpTriangleList_[1]->empty();
    }
}

/* */
void TriangleSet::getAdjTriangles2_(int vertexId, GrowArray *adjTrianglesList)
{
    int                 i, index, n;
    AdjTrianglesIndex   *pAdjTrIndex;

    pAdjTrIndex = (AdjTrianglesIndex *) adjTrianglesIndex_->getElement(vertexId);

    n = pAdjTrIndex->lastIndex - pAdjTrIndex->startIndex + 1;
    if(n > 0)
    {
        index = pAdjTrIndex->startIndex;
        for(i = 0; i < n; i++)
        {
            adjTrianglesList->appendElement(adjTriangles_->getElement(index));
            index++;
        }
    }
}

/* appends adjacent triangles to exisiting list */

/* the triangle ids in list are sorted in ascending order */
void TriangleSet::getAdjTriangles_(int vertexId, GrowArray *adjTrianglesList)
{
    int     i, n, j;
    int     l, m, r;
    BOOL    found;
    int     trId;

    for(i = 0; i < 3; i++)
    {
        /* binary-search vertexId in trianglesSorted_[i] */
        l = 0;
        r = nf_ - 1;
        found = FALSE;
        while((l <= r) && (!found))
        {
            if(l == r)
                m = l;
            else
                m = (l + r) >> 1;
            if(trianglesSorted_[i][m].vertexId == vertexId)
                found = TRUE;
            else
            {
                if(trianglesSorted_[i][m].vertexId < vertexId)
                    l = m + 1;
                else
                    r = m - 1;
            }
        }

        /* BUGFIX !!

       case l==r was unhandled.

    */
        if(found)
        {
            adjTrianglesList->appendElement(&trianglesSorted_[i][m].triangleId);

            /* walk left and right and collect traingles with vertexId */
            l = m - 1;
            while((l >= 0) && (trianglesSorted_[i][l].vertexId == vertexId))
            {
                adjTrianglesList->appendElement(&trianglesSorted_[i][l].triangleId);
                l--;
            }

            r = m + 1;
            while((r < nf_) && (trianglesSorted_[i][r].vertexId == vertexId))
            {
                adjTrianglesList->appendElement(&trianglesSorted_[i][r].triangleId);
                r++;
            }
        }
    }                           /* for */

    n = adjTrianglesList->getNumElements();

    if(n > 0)
    {
        /* now sort out doubled triangles */
        n = adjTrianglesList->getNumElements();
        for(i = 0; i < n; i++)
        {
            trId = *((int *) adjTrianglesList->getElement(i));
            if(!doubleTriangle_[0]->getBit(trId))
            {
                tmpTriangleList_[0]->appendElement(&trId);
                doubleTriangle_[0]->setBit(trId, 1);
            }
        }

        n = tmpTriangleList_[0]->getNumElements();

        if(n > 0)
        {
            adjTrianglesList->copy(tmpTriangleList_[0]);
        }

        /* now sort the list */
        if(n > 1)
        {
            adjTrianglesList->sort(0, -1, compareInts);
        }

        /* clear used arrays */
        doubleTriangle_[0]->empty();
        tmpTriangleList_[0]->empty();
    }
}

/* */
void TriangleSet::setNumAdjVertices_(void)
{
    int i;
    for(i = 0; i < np_; i++)
    {
        getAdjacentVertices(i, tmpVertexList_);
        numAdjVertices_[i] = tmpVertexList_->getNumElements();
        tmpVertexList_->empty();
    }
}

/* */
void TriangleSet::init(int np, int nf)
{
    int i, j;

    dataComplete_ = FALSE;
    boundingBoxCalculated_ = FALSE;

    if(faces_)
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
        GEOMARK_DELETE(triangleHashTable_);
        triangleHashTable_ = NULL;
    }

    if(triangleHashMarked_)
    {
        GEOMARK_DELETE(triangleHashMarked_);
        triangleHashMarked_ = NULL;
    }

    np_ = 0;
    nf_ = 0;
    numPatterns_ = 0;
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

    if(numAdjVertices_)
    {
        GEOMARK_FREE(numAdjVertices_);
        numAdjVertices_ = NULL;
    }

    for(i = 0; i < 2; i++)
    {
        doubleTriangle_[i] = new GEOMARK_BitArray(nf);
    }

    for(i = 0; i < 4; i++)
    {
        tmpTriangleList_[i] = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(nf),
                                            sizeof(int));
    }

    tmpVertexList_ = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(np),
                                   sizeof(int));
    verticesCopied_ = new GEOMARK_BitArray(np);
    for(i = 0; i < 1; i++)
    {
        verticesMarked_[i] = new GEOMARK_BitArray(np);
    }

    for(i = 0; i < 3; i++)
    {
        trianglesSorted_[i] = (SortTrianglesByVertexIndicesEntry *) malloc(nf * sizeof(SortTrianglesByVertexIndicesEntry));
    }

    adjTriangles_ = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(nf * 3),
                                  sizeof(int));
    adjTrianglesIndex_ = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(np),
                                       sizeof(AdjTrianglesIndex));
    numAdjVertices_ = (int *) malloc(np * sizeof(int));

    faces_ = (TriangleFace *) malloc(nf * sizeof(TriangleFace));
    points_ = (TinyVector **) malloc(np * sizeof(TinyVector *));

    triangleHashTable_ = new HashArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(nf), 9);
    triangleHashMarked_ = new GEOMARK_BitArray(nf);

    for(i = 0; i < np; i++)
    {
        points_[i] = new TinyVector();
    }

    np_ = np;
    nf_ = nf;
    llf_ = TinyVector(0, 0, 0, 1);
    urb_ = TinyVector(0, 0, 0, 1);
    bbDiameter_ = 0;
}

/* */
void TriangleSet::initXZGrid(double widthX, double heightZ, int numXSegments,
                             int numZSegments)
{
    int         numVertices, numTriangles;
    double      deltaX, deltaZ;
    int         i, j, faceId, vertexId, vertexRowStartId;
    double      xPos, zPos;
    TinyVector  v;
    int         numVerticesInRow;

    numVertices = (numXSegments + 1) * (numZSegments + 1);
    numTriangles = numXSegments * numZSegments * 2;
    init(numVertices, numTriangles);

    deltaX = widthX / numXSegments;
    deltaZ = heightZ / numZSegments;

    /* set vertices */
    zPos = 0;
    vertexId = 0;
    for(i = 0; i <= numZSegments; i++)
    {
        xPos = 0;
        for(j = 0; j <= numXSegments; j++)
        {
            v[0] = xPos;
            v[1] = 0;
            v[2] = zPos;
            setVertex(vertexId, v);
            vertexId++;
            xPos += deltaX;
        }

        zPos += deltaZ;
    }

    /* set faces */
    numVerticesInRow = numXSegments + 1;

    faceId = 0;
    vertexId = 0;
    vertexRowStartId = 0;
    for(i = 0; i < numZSegments; i++)
    {
        vertexId = vertexRowStartId;
        for(j = 0; j < numXSegments; j++)
        {
            setFace(faceId, vertexId, vertexId + numVerticesInRow,
                    vertexId + numVerticesInRow + 1);
            setFace(faceId + 1, vertexId, vertexId + numVerticesInRow + 1,
                    vertexId + 1);

            vertexId++;
            faceId += 2;
        }

        vertexRowStartId += numVerticesInRow;
    }

    generateAdjList_();
    dataComplete_ = TRUE;
    setNumAdjVertices_();
}

/* */
void TriangleSet::copySet(TriangleSet *sourceTs)
{
    int i, nv, nf;
    int vertexIds[3];

    nv = sourceTs->vertices();
    nf = sourceTs->triangles();

    init(nv, nf);

    for(i = 0; i < nv; i++)
    {
        setVertex(i, sourceTs->getVertex(i));
    }

    for(i = 0; i < nf; i++)
    {
        sourceTs->getTriangleVertices(i, vertexIds);
        setFace(i, vertexIds[0], vertexIds[1], vertexIds[2]);
    }

    generateAdjList_();
    dataComplete_ = TRUE;
    setNumAdjVertices_();
}

/* */
void TriangleSet::meshComplete(void)
{
    generateAdjList_();
    dataComplete_ = TRUE;
    setNumAdjVertices_();
}

/* */
int TriangleSet::getTriangleHash(int trId)
{
    int             rv, h, vIds[3];
    unsigned char   key[9];
    int             hashValue;

    getTriangleVertices(trId, vIds);

    /* order vertex ids acending */
    if(vIds[0] > vIds[1])
    {
        h = vIds[0];
        vIds[0] = vIds[1];
        vIds[1] = h;
    }

    if(vIds[1] > vIds[2])
    {
        h = vIds[1];
        vIds[1] = vIds[2];
        vIds[2] = h;
    }

    if(vIds[0] > vIds[1])
    {
        h = vIds[0];
        vIds[0] = vIds[1];
        vIds[1] = h;
    }

    memcpy((void *) &key[0], (void *) &vIds[0], 3);
    memcpy((void *) &key[3], (void *) &vIds[1], 3);
    memcpy((void *) &key[6], (void *) &vIds[2], 3);

    rv = triangleHashTable_->searchElement((void *) &key[0], (void **) &hashValue);

    if(!rv)
    {
        /* old pattern */
        return hashValue;
    }
    else
        return -1;
}

/* */
int TriangleSet::getNumAdjVertices(int vertexId)
{
    if(!dataComplete_)
    {
        generateAdjList_();
        dataComplete_ = TRUE;   /* IMPORTANT: this must precede
                          next call, otherwise endless recursion !!! */
        setNumAdjVertices_();
    }

    return numAdjVertices_[vertexId];
}

/* */
int TriangleSet::getNumAdjTriangles(int vertexId)
{
    AdjTrianglesIndex   *adjTrIndex;

    if(!dataComplete_)
    {
        generateAdjList_();
        dataComplete_ = TRUE;
        setNumAdjVertices_();
    }

    adjTrIndex = (AdjTrianglesIndex *) adjTrianglesIndex_->getElement(vertexId);
    return adjTrIndex->lastIndex - adjTrIndex->startIndex + 1;
}

/* */
int TriangleSet::getNumVerticesWithValence(int valence)
{
    int                 i, n, numVertices = 0;

    ofstream            file("valence.dat");
    GEOMARK_BitArray    *vertexMarked;
    int                 vIds[3];

    vertexMarked = new GEOMARK_BitArray(this->vertices());

    n = np_;
    for(i = 0; i < n; i++)
    {
        if(this->getNumAdjVertices(i) == valence)
        {
            numVertices++;

            //file << i << " " << i << "\n";
            vertexMarked->setBit(i, 1);
        }
    }

    n = nf_;
    for(i = 0; i < n; i++)
    {
        getTriangleVertices(i, vIds);
        if(vertexMarked->getBit(vIds[0]) &&
           vertexMarked->getBit(vIds[1]) &&
           vertexMarked->getBit(vIds[2]))
        {
            file << i << "\n";
        }
    }

    file.close();

    GEOMARK_DELETE(vertexMarked);
    vertexMarked = NULL;

    return numVertices;
}
#if 0
/*
int TriangleSet::importDXF(FILE *file)
{
  return 0;
}
*/
int TriangleSet::importOBJ(std::istream &file)
{
    import_(IMPORT_FILE_TYPE_OBJ, file, 0);
    return 0;
}

/* */
int TriangleSet::exportOBJ(std::ostream &file, int precision)
{
    int         i, n, m;
    TinyVector  v;
    int         i1, i2, i3;

    n = vertices();
    m = triangles();

    //file << n << " " << m << "\n";
    //printf("%d\n",ios::precision());
    for(i = 0; i < n; i++)
    {
        v = getVertex(i);

        //v.dump();
        file <<
            "v " <<
            setprecision(precision) <<
            v[0] <<
            " " <<
            v[1] <<
            " " <<
            v[2] <<
            "\n";
    }

    for(i = 0; i < m; i++)
    {
        i1 = getTriangleVertex(i, 0);
        i2 = getTriangleVertex(i, 1);
        i3 = getTriangleVertex(i, 2);
        file << "f " << i1 + 1 << " " << i2 + 1 << " " << i3 + 1 << "\n";

        //cout << "f " << i1+1 << " " << i2+1 << " " << i3+1 << "\n";
    }

    return 0;
}

/*
  Comments for editing exported .fig files with
  javafig:

  1.

  When generating the fig-file we do not perform
  any occlusion culling. Instead we simply draw
  triangular faces in an order depending on the
  z-value of the center of mass of each triangle.
  This order MUST be maintained in further processing
  steps. javafig stores trinagular faces in reverse order.
  The reason for this seems to be that the fig2dev utility
  inverts the ordering, so everything is ok for the .eps file.
  But, if we import the .fig, the ordering is reversed.
  As a workaround I wrote the utility figreverse which
  reverses the contents of a fig-file.
  So after saving a fig-file this utility has to be applied
  before loading it again.

  2.

  If we want to highlight/mark certain edges or faces within
  javafig we encounter problems: It is not possible to
  seperate between objects by selecting vertices.
  So the only applicable way for highlighting is to draw above
  the model. Choose depth 99 or lower for this purpose.
  
  3.

  There are problems with javafig and coloring faces. A face
  is always filled with the pattern and color black or white.
  If we want to fill a face e.g. with red we have to perform
  following edits in the .eps file:

  A. search for color representing read
     col4 {1.000 0.000 0.000 srgb} bind def
  B. find face in eps file, change expression
     "col4 0.00 shd ef"
     to
	 "col4 ef"
*/
int TriangleSet::export2DVecFormat(ostream &file, int format,
                                   GEOMARK_BitArray *markedFaces)
{
    TinyMatrix          fitTrans, rotX;
    SortTrianglesByComZ *sortedTriangleZValues;
    TinyVector          *fitVertices;
    int                 i, j;
    TinyVector          v, com, ip;
    int                 vIds[3];
    double              x, y, alpha;
    double              coords[3][2];
    int                 icoords[3][2];
    char                line[1024];
    FILE                *f;
    char                headerFilename[255];
    double              fovyZ, planeZ;

    markedFaces = NULL;

    TinyVector  fov, planeOrigin, planeRv1, planeRv2;

    fovyZ = 2;
    planeZ = 1;                 /* HARDCODED */

    fov[2] = fovyZ;
    planeOrigin[2] = planeZ;
    planeRv1[0] = 1;
    planeRv2[1] = 1;

#if 0
    strcpy(headerFilename, "header_eps.dat");   /* HARDCODED */
    f = fopen(headerFilename, "r");
    while(fgets(line, 1023, f) != NULL)
        //printf("%s",line);
        file << line;
    file << "\n";
    fclose(f);
#endif
    if(format == VEC2DFORMAT_EPS)
    {
        file << "%!PS-Adobe-2.0 EPSF-2.0\n";
        file << "%%Title: \n";
        file << "%%Creator: gnuplot\n";
        file << "%%DocumentFonts: (atend)\n";
        file << "%%BoundingBox: 0 0 400 400\n";
        file << "%%Orientation: Portrait\n";
        file << "%%EndComments\n";
        file << "/m {moveto} def\n";
        file << "/n {newpath} def\n";
        file << "/l {lineto} def\n";
        file << "/cp {closepath} def\n";
        file << "/s {stroke} def\n";
        file << "/f {fill} def\n";
        file << ".500 setlinewidth\n";

        // 0=miter joins, 1=round joins, 2=bevel joins
        file << "1 setlinejoin\n";

        // e.g. 10="sharp edge", 2="cut edge"
        file << "3 setmiterlimit\n";
        file << "%%EndProlog\n";
    }
    else if(format == VEC2DFORMAT_FIG31)
    {
        file << "#FIG 3.1\n";                   // format
        file << "Landscape\n";                  // orientation (Lanscape,Portrait)
        file << "Center\n";                     // justification (Center,Flush Left)
        file << "Inches\n";                     // units (Inches, Metric)
        file << "1200 2\n";                     // fig-resolution, coordinate system

        // system: 1 origin lower left
        //         2 origin upper left
    }

    sortedTriangleZValues = (SortTrianglesByComZ *) malloc(nf_ * sizeof(SortTrianglesByComZ));
    fitVertices = (TinyVector *) malloc(np_ * sizeof(TinyVector));

    /* fitInUnitCube() also moves object center of mass into origin ! */
    fitInUnitCube(&fitTrans, FALSE);

    rotX.rot(-90., ROT_X);
    fitTrans = rotX * fitTrans;

    /* fit vertices into unit cube */
    for(i = 0; i < np_; i++)
    {
        fitVertices[i] = fitTrans * getVertex(i);
    }

    /* sort faces based on Z coordinate of their center of mass */
    for(i = 0; i < nf_; i++)
    {
        sortedTriangleZValues[i].triangleId = i;
        getTriangleVertices(i, vIds);
        com =
            (
                fitVertices[vIds[0]] +
                fitVertices[vIds[1]] +
                fitVertices[vIds[2]]
            ) /
            3.;
        sortedTriangleZValues[i].comZ = com[2];
    }

    qsort(sortedTriangleZValues, nf_, sizeof(SortTrianglesByComZ),
          compareFacesByComZ);

    //  for (i=0;i<nf_;i++) {
    //    printf("%d, %f\n",sortedTriangleZValues[i].triangleId,
    //		sortedTriangleZValues[i].comZ);
    //  }
    /* perform perspective projection of points onto plane */
    for(i = 0; i < nf_; i++)
    {
        //for (i=nf_-1;i>=0;i--) {
        if(format == VEC2DFORMAT_EPS)
        {
            getTriangleVertices(sortedTriangleZValues[i].triangleId, vIds);
        }
        else if(format == VEC2DFORMAT_FIG31)
        {
            getTriangleVertices(sortedTriangleZValues[nf_ - i - 1].triangleId,
                                vIds);
        }

        for(j = 0; j < 3; j++)
        {
            v = fitVertices[vIds[j]];
            intersectionLinePlane(v, fov - v, planeOrigin, planeRv1, planeRv2,
                                  &alpha, &ip);

            if(format == VEC2DFORMAT_EPS)
            {
                /* get screen coords */
                /* correct scale factor is 600, but we leave
	      some space */
                /* it may be necessary to remove space by hand,
	      just edit bounding box, e.g. increasing
	      second value removes lowe space */
                /* ip is in range -1/3 to 1/3 */
                coords[j][0] = 200 + (ip[0] * 595);
                coords[j][1] = 200 + (ip[1] * 595);
            }
            else if(format == VEC2DFORMAT_FIG31)
            {
                /* we draw in a qadrat 10 by 10 cm size.
		  1 cm is 450 fig-units */
                icoords[j][0] = 2250 + (ip[0] * 6750);
                icoords[j][1] = 2250 - (ip[1] * 6750);
            }
        }

        if(format == VEC2DFORMAT_EPS)
        {
            if((markedFaces) &&
               (markedFaces->getBit(sortedTriangleZValues[i].triangleId)))
            {
                file <<
                    "n " <<
                    coords[0][0] <<
                    " " <<
                    coords[0][1] <<
                    " m " <<
                    coords[1][0] <<
                    " " <<
                    coords[1][1] <<
                    " l " <<
                    coords[2][0] <<
                    " " <<
                    coords[2][1] <<
                    " l " <<
                    "cp .5 setgray f\n";
            }
            else
            {
                file <<
                    "n " <<
                    coords[0][0] <<
                    " " <<
                    coords[0][1] <<
                    " m " <<
                    coords[1][0] <<
                    " " <<
                    coords[1][1] <<
                    " l " <<
                    coords[2][0] <<
                    " " <<
                    coords[2][1] <<
                    " l " <<
                    "cp 1.0 setgray f\n";
            }

            file <<
                "n " <<
                coords[0][0] <<
                " " <<
                coords[0][1] <<
                " m " <<
                coords[1][0] <<
                " " <<
                coords[1][1] <<
                " l " <<
                coords[2][0] <<
                " " <<
                coords[2][1] <<
                " l " <<
                "cp 0.0 setgray s\n";
        }
        else if(format == VEC2DFORMAT_FIG31)
        {
            /* explanation of line preceeding coordinates:
	  
		Object definition POLYLINE:

        object code (always 2)
		sub_type (1: polyline, 2:box, 3:polygon)
		line_style
		thickness (multiple of 1/80 inch)
		pen-color
		fill-color
		depth
		pen-style
		area fill (-1: no fill, 0: black, 1-19: shades of grey, 20: black)
		style-val
		join-style (2: round)
		cap-style (1: round)
		radius
		forward-arrow
		backward-arrow
		npoints
	  */
            if((markedFaces) &&
               (markedFaces->getBit(sortedTriangleZValues[i].triangleId)))
            {
                file << "2 1 0 1 0 0 100 1 5 6.4 2 1 12 0 0 4\n";
            }
            else
            {
                file << "2 1 0 1 0 0 100 1 0 6.4 2 1 12 0 0 4\n";
            }

            file << "        ";
            file << icoords[0][0] << " " << icoords[0][1] << " ";
            file << icoords[1][0] << " " << icoords[1][1] << " ";
            file << icoords[2][0] << " " << icoords[2][1] << " ";
            file << icoords[0][0] << " " << icoords[0][1] << "\n";
        }
    }                           /* for i */

    if(format == VEC2DFORMAT_EPS)
        file << "showpage\n";

    GEOMARK_FREE(sortedTriangleZValues);
    GEOMARK_FREE(fitVertices);

    return 0;
}

/* */
int TriangleSet::exportQhull(ostream &file)
{
    int         i;
    int         nv;
    TinyVector  v;

    nv = vertices();
    file << "3 " << nv << "\n";
    for(i = 0; i < nv; i++)
    {
        v = getVertex(i);
        file << v[0] << " " << v[1] << " " << v[2] << "\n";
    }

    return 0;
}

/* */
int TriangleSet::exportHoppe(ostream &file)
{
    int         i, nv, nf;
    TinyVector  v;
    int         vIds[3];

    nv = vertices();
    nf = triangles();

    for(i = 0; i < nv; i++)
    {
        v = getVertex(i);
        file <<
            "Vertex " <<
            i +
            1 <<
            " " <<
            v[0] <<
            " " <<
            v[1] <<
            " " <<
            v[2] <<
            "\n";
    }

    for(i = 0; i < nf; i++)
    {
        getTriangleVertices(i, vIds);
        file <<
            "Face " <<
            i +
            1 <<
            " " <<
            vIds[0] +
            1 <<
            " " <<
            vIds[1] +
            1 <<
            " " <<
            vIds[2] +
            1 <<
            "\n";
    }

    return 0;
}

/* */
int TriangleSet::importOFF(istream &file)
{
    import_(IMPORT_FILE_TYPE_OFF, file, 0);

    return 0;
}

/* */
void TriangleSet::import_(int fileType, istream &file, int version)
{
    int             numVertices, numTriangles, i, j, n;
    int             mask = 0;
    int             numVerticesPolygon;
    char            dummyString[255];

    double          x, y, z;
    TinyVector      u, v, w;

    int             ind[3];
    int             index, rv, h;
    unsigned char   key[9];

    char            lineId[256];
    char            groupDescr[256];

    //BOOL parseError;
    double          v1, v2, v3;
    int             f1, f2, f3;
    int             vertexCount, triangleCount;
    int             dummyInt;

    dataComplete_ = FALSE;

    // reading deletes the existing data, it is not additive
    if(faces_)
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
        GEOMARK_DELETE(triangleHashTable_);
        triangleHashTable_ = NULL;
    }

    if(triangleHashMarked_)
    {
        GEOMARK_DELETE(triangleHashMarked_);
        triangleHashMarked_ = NULL;
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

    if(numAdjVertices_)
    {
        GEOMARK_FREE(numAdjVertices_);
        numAdjVertices_ = NULL;
    }

    for(i = 0; i < 3; i++)
    {
        if(trianglesSorted_[i])
        {
            GEOMARK_FREE(trianglesSorted_[i]);
            trianglesSorted_[i] = NULL;
        }
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

    np_ = 0;
    nf_ = 0;
    llf_ = TinyVector(0, 0, 0, 1);
    urb_ = TinyVector(0, 0, 0, 1);
    bbDiameter_ = 0;

    numPatterns_ = 0;

    numVertices = 0;
    numTriangles = 0;

    if(fileType == IMPORT_FILE_TYPE_VRS)
    {
        file >> numVertices >> numTriangles;
    }
    else if(fileType == IMPORT_FILE_TYPE_OFF)
    {
        file >> dummyString;    /* OFF */
        file >> numVertices >> numTriangles >> dummyInt;
    }
    else
    {                           /* fileType==IMPORT_FILE_TYPE_OBJ */
        while(!file.eof())
        {
            file >> lineId;
            if(!strcmp(lineId, "v"))
            {
                file >> v1 >> v2 >> v3;
                numVertices++;
            }
            else if(!strcmp(lineId, "f"))
            {
                file >> f1 >> f2 >> f3;
                numTriangles++;
            }
            else if(!strcmp(lineId, "g"))
            {
                file >> groupDescr;
            }
            else if(!strcmp(lineId, "vn"))
            {
                file >> v1 >> v2 >> v3;
            }
            else
            {
                /* just skip */
            }

            lineId[0] = '\0';
        }

        file.clear();
        file.seekg(0);
    }

    faces_ = (TriangleFace *) malloc(numTriangles * sizeof(TriangleFace));
    points_ = (TinyVector **) malloc(numVertices * sizeof(TinyVector *));

    triangleHashTable_ = new HashArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(numTriangles),
                                           9);
    triangleHashMarked_ = new GEOMARK_BitArray(numTriangles);

    adjTriangles_ = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(numTriangles * 3),
                                      sizeof(int));
    adjTrianglesIndex_ = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(numVertices),
                                           sizeof(AdjTrianglesIndex));
    numAdjVertices_ = (int *) malloc(numVertices * sizeof(int));

    for(i = 0; i < 2; i++)
    {
        doubleTriangle_[i] = new GEOMARK_BitArray(numTriangles);
    }

    for(i = 0; i < 3; i++)
    {
        trianglesSorted_[i] = (SortTrianglesByVertexIndicesEntry *) malloc(numTriangles * sizeof(SortTrianglesByVertexIndicesEntry));
    }

    for(i = 0; i < 4; i++)
    {
        tmpTriangleList_[i] = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(numTriangles),
                                                sizeof(int));
    }

    tmpVertexList_ = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(numVertices),
                                   sizeof(int));

    verticesCopied_ = new GEOMARK_BitArray(numVertices);
    for(i = 0; i < 1; i++)
    {
        verticesMarked_[i] = new GEOMARK_BitArray(numVertices);
    }

    for(i = 0; i < numVertices; i++)
    {
        points_[i] = new TinyVector();
    }

    if((fileType == IMPORT_FILE_TYPE_VRS) ||
       (fileType == IMPORT_FILE_TYPE_OFF))
    {
        /* no vector has been called right now */
        for(i = 0; i < numVertices; i++)
        {
            if(fileType == IMPORT_FILE_TYPE_VRS)
                file >> x >> y >> z;
            else
                file >> x >> y >> z;
            v[0] = x;
            v[1] = y;
            v[2] = z;
            np_++;
            *points_[i] = v;

            if(i == 0)
            {
                llf_ = v;
                urb_ = v;
            }
            else
            {
                if(v[0] < llf_[0])
                    llf_[0] = v[0];
                if(v[1] < llf_[1])
                    llf_[1] = v[1];
                if(v[2] < llf_[2])
                    llf_[2] = v[2];
                if(v[0] > urb_[0])
                    urb_[0] = v[0];
                if(v[1] > urb_[1])
                    urb_[1] = v[1];
                if(v[2] > urb_[2])
                    urb_[2] = v[2];
            }
        }

        i = 0;
        n = numTriangles;
        while(i < n)
        {
            if(version == 1)
            {
                file >> ind[0] >> ind[1] >> ind[2];

                faces_[i].ind[0] = ind[0];
                faces_[i].ind[1] = ind[1];
                faces_[i].ind[2] = ind[2];

                //adjTriangles_[ind[0]]->appendElement(&i);
                //adjTriangles_[ind[1]]->appendElement(&i);
                //adjTriangles_[ind[2]]->appendElement(&i);
                /* order vertex ids acending */
                if(ind[0] > ind[1])
                {
                    h = ind[0];
                    ind[0] = ind[1];
                    ind[1] = h;
                }

                if(ind[1] > ind[2])
                {
                    h = ind[1];
                    ind[1] = ind[2];
                    ind[2] = h;
                }

                if(ind[0] > ind[1])
                {
                    h = ind[0];
                    ind[0] = ind[1];
                    ind[1] = h;
                }

                memcpy((void *) &key[0], (void *) &ind[0], 3);
                memcpy((void *) &key[3], (void *) &ind[1], 3);
                memcpy((void *) &key[6], (void *) &ind[2], 3);

                rv = triangleHashTable_->searchElement((void *) &key[0], NULL);
                if(rv)
                {
                    /* new pattern */
                    triangleHashTable_->insertElement((void *) &key[0],
                                                      (void *) numPatterns_);
                    numPatterns_++;
                }

                nf_++;
                i++;
            }
            else
            {
                file >> numVerticesPolygon;
                for(j = 0; j < numVerticesPolygon; j++)
                {
                    file >> index;
                    if(j < 3)
                        ind[j] = index;
                }

                if(numVerticesPolygon == 3)
                {
                    faces_[i].ind[0] = ind[0];
                    faces_[i].ind[1] = ind[1];
                    faces_[i].ind[2] = ind[2];

                    //adjTriangles_[ind[0]]->appendElement(&i);
                    //adjTriangles_[ind[1]]->appendElement(&i);
                    //adjTriangles_[ind[2]]->appendElement(&i);
                    /* order vertex ids acending */
                    if(ind[0] > ind[1])
                    {
                        h = ind[0];
                        ind[0] = ind[1];
                        ind[1] = h;
                    }

                    if(ind[1] > ind[2])
                    {
                        h = ind[1];
                        ind[1] = ind[2];
                        ind[2] = h;
                    }

                    if(ind[0] > ind[1])
                    {
                        h = ind[0];
                        ind[0] = ind[1];
                        ind[1] = h;
                    }

                    memcpy((void *) &key[0], (void *) &ind[0], 3);
                    memcpy((void *) &key[3], (void *) &ind[1], 3);
                    memcpy((void *) &key[6], (void *) &ind[2], 3);

                    rv = triangleHashTable_->searchElement((void *) &key[0],
                                                           NULL);
                    if(rv)
                    {
                        /* new pattern */
                        triangleHashTable_->insertElement((void *) &key[0],
                                                          (void *) numPatterns_);
                        numPatterns_++;
                    }

                    nf_++;
                    i++;
                }
                else
                    n--;        /* we simply ingnore faces with more than three points */
            }
        }                       /* while i */
    }
    else
    {                           /* fileType==IMPORT_FILE_TYPE_OBJ */
        vertexCount = 0;
        triangleCount = 0;
        while(!file.eof())
        {
            file >> lineId;
            if(!strcmp(lineId, "v"))
            {
                file >> v1 >> v2 >> v3;
                v[0] = v1;
                v[1] = v2;
                v[2] = v3;

                np_++;
                *points_[vertexCount] = v;

                if(vertexCount == 0)
                {
                    llf_ = v;
                    urb_ = v;
                }
                else
                {
                    if(v[0] < llf_[0])
                        llf_[0] = v[0];
                    if(v[1] < llf_[1])
                        llf_[1] = v[1];
                    if(v[2] < llf_[2])
                        llf_[2] = v[2];
                    if(v[0] > urb_[0])
                        urb_[0] = v[0];
                    if(v[1] > urb_[1])
                        urb_[1] = v[1];
                    if(v[2] > urb_[2])
                        urb_[2] = v[2];
                }

                vertexCount++;
            }
            else if(!strcmp(lineId, "f"))
            {
                file >> ind[0] >> ind[1] >> ind[2];

                ind[0] -= 1;
                ind[1] -= 1;
                ind[2] -= 1;

                faces_[triangleCount].ind[0] = ind[0];
                faces_[triangleCount].ind[1] = ind[1];
                faces_[triangleCount].ind[2] = ind[2];

                /* order vertex ids acending */
                if(ind[0] > ind[1])
                {
                    h = ind[0];
                    ind[0] = ind[1];
                    ind[1] = h;
                }

                if(ind[1] > ind[2])
                {
                    h = ind[1];
                    ind[1] = ind[2];
                    ind[2] = h;
                }

                if(ind[0] > ind[1])
                {
                    h = ind[0];
                    ind[0] = ind[1];
                    ind[1] = h;
                }

                memcpy((void *) &key[0], (void *) &ind[0], 3);
                memcpy((void *) &key[3], (void *) &ind[1], 3);
                memcpy((void *) &key[6], (void *) &ind[2], 3);

                rv = triangleHashTable_->searchElement((void *) &key[0], NULL);
                if(rv)
                {
                    /* new pattern */
                    triangleHashTable_->insertElement((void *) &key[0],
                                                      (void *) numPatterns_);
                    numPatterns_++;
                }

                nf_++;
                triangleCount++;
            }
            else if(!strcmp(lineId, "g"))
            {
                file >> groupDescr;
            }
            else if(!strcmp(lineId, "vn"))
            {
                file >> v1 >> v2 >> v3;
            }

            lineId[0] = '\0';
        }                       /* while */
    }                           /* fileType==IMPORT_FILE_TYPE_OBJ */

    //sortTriangles_();
    generateAdjList_();
    dataComplete_ = TRUE;

    bbDiameter_ = tv_abs(urb_ - llf_);
    boundingBoxCalculated_ = TRUE;

    setNumAdjVertices_();
}

/* */
int TriangleSet::importVRS(istream &file, int version)
{
    import_(IMPORT_FILE_TYPE_VRS, file, version);
    return 0;
}

/* */
int TriangleSet::exportVRS(ostream &file, int version, int precision)
{
    int         i, n, m;
    TinyVector  v;
    int         i1, i2, i3;

    n = vertices();
    m = triangles();

    file << n << " " << m << "\n";

    for(i = 0; i < n; i++)
    {
        v = getVertex(i);
        file <<
            setprecision(precision) <<
            v[0] <<
            " " <<
            v[1] <<
            " " <<
            v[2] <<
            "\n";

        //file << v[0] << " " << v[1] << " " << v[2] << "\n";
    }

    for(i = 0; i < m; i++)
    {
        i1 = getTriangleVertex(i, 0);
        i2 = getTriangleVertex(i, 1);
        i3 = getTriangleVertex(i, 2);
        if(version == 1)
        {
            file << i1 << " " << i2 << " " << i3 << "\n";
        }
        else
        {
            file << "3 " << i1 << " " << i2 << " " << i3 << "\n";
        }
    }

    return 0;
}
#endif
/* */
int TriangleSet::vertices(void)
{
    return np_;
}

/* */
int TriangleSet::triangles(void)
{
    return nf_;
}

/* */
void TriangleSet::getBoundingBox(TinyVector *llf, TinyVector *urb,
                                 BOOL forceRecalculation)
{
    int         i;
    TinyVector  v;

    if((boundingBoxCalculated_) && (!forceRecalculation))
    {
        *llf = llf_;
        *urb = urb_;
    }
    else
    {
        /* calculate it now */
        for(i = 0; i < np_; i++)
        {
            v = *points_[i];
            if(i == 0)
            {
                llf_ = v;
                urb_ = v;
            }
            else
            {
                if(v[0] < llf_[0])
                    llf_[0] = v[0];
                if(v[1] < llf_[1])
                    llf_[1] = v[1];
                if(v[2] < llf_[2])
                    llf_[2] = v[2];
                if(v[0] > urb_[0])
                    urb_[0] = v[0];
                if(v[1] > urb_[1])
                    urb_[1] = v[1];
                if(v[2] > urb_[2])
                    urb_[2] = v[2];
            }
        }                       /* for */

        *llf = llf_;
        *urb = urb_;
        boundingBoxCalculated_ = TRUE;
        bbDiameter_ = tv_abs(urb_ - llf_);
    }
}

/* */
double TriangleSet::getBoundingBoxDiameter(BOOL forceRecalculation)
{
    TinyVector  llf, urb;

    if((boundingBoxCalculated_) && (!forceRecalculation))
    {
        return bbDiameter_;
    }

    this->getBoundingBox(&llf, &urb, forceRecalculation);
    return bbDiameter_;
}

/* */
double TriangleSet::getLongestEdgeOfBoundingBox(BOOL forceRecalculation)
{
    int         i;
    double      edgeLength, maxEdgeLength = 0;
    TinyVector  llf, urb;

    this->getBoundingBox(&llf, &urb, forceRecalculation);
    for(i = 0; i < 3; i++)
    {
        edgeLength = fabs(llf[i] - urb[i]);
        if(edgeLength > maxEdgeLength)
            maxEdgeLength = edgeLength;
    }

    return maxEdgeLength;
}

/* always recalculation */
double TriangleSet::getMaxDistTwoVertices(void)
{
    double      dist, maxDist = 0;
    int         i, j, n;
    TinyVector  v1, v2;

    n = this->vertices();
    for(i = 0; i < n - 1; i++)
    {
        v1 = this->getVertex(i);
        for(j = i + 1; j < n; j++)
        {
            v2 = this->getVertex(j);
            dist = tv_abs(v1 - v2);
            if(dist > maxDist)
                maxDist = dist;
        }
    }

    return maxDist;
}

/* */
int TriangleSet::reorientNormals(BOOL queryOnly)
{
#ifdef COLDET_PRESENT           /* to prevent unecessary linking with coldet library */
    int                     i;
    int                     numVertices;
    int                     normalOrientationStatus;
    int                     angleX = 60.;   /* HARDCODED */
    double                  angle;
    int                     numNormalsInward, numNormalsOutward;
    double                  bbDiameter;
    TinyVector              dir, v, com, normal;
    int                     rv;
    int                     intersectedFaceId;
    TinyVector              intersectionPoint;
    double                  intersectionDistance;
    SpatialSortTriangles    *sst;

    /* our method detrmining correctly oriented normals:
  
    We cast rays from outside the bounding box onto the
	model surface. We select all interescted faces for which 
	the angle between ray and surface normal is in [0-x,0+x] or 
	[180-x,180+x]. If for the mayority of selected faces the
	angle is in [0-x,0+x] we consider the normals to be
	falsely oriented.
  */
    TriangleSet             *polyhTs = new TriangleSet();
    ifstream                polyhFile("bin100.obj");
    polyhTs->importOBJ(polyhFile);
    polyhFile.close();

    sst = new SpatialSortTriangles(this);
    numNormalsInward = 0;
    numNormalsOutward = 0;

    com = centerOfMass();
    bbDiameter = getBoundingBoxDiameter(TRUE);

    numVertices = polyhTs->vertices();
    for(i = 0; i < numVertices; i++)
    {
        v = polyhTs->getVertex(i);
        v.normalize();
        v = com + (v * bbDiameter);
        dir = v * -1.;

        rv = sst->getRayNearestFaceIntersection(v, dir, &intersectedFaceId,
                                                &intersectionPoint,
                                                &intersectionDistance, FALSE);

        if(rv != -1)
        {
            normal = triangleNormal(intersectedFaceId);
            if((normal[0] != 0) && (normal[1] != 0) && (normal[2] != 0))
            {
                angle = fabs(DEGREE(dir.angle(normal)));
                if(angle <= angleX)
                {
                    numNormalsInward++;
                }
                else if(angle >= 180. - angleX)
                {
                    numNormalsOutward++;
                }
            }
        }
    }

    normalOrientationStatus = 0;
    if((numNormalsInward + numNormalsOutward) > 20)
    {                   /* HARDCODED */
        if((
               (
               numNormalsInward /
               ((double) (numNormalsInward + numNormalsOutward))
   ) > 0.7
       ))
            normalOrientationStatus = -1;
    }

    GEOMARK_DELETE(polyhTs);
    GEOMARK_DELETE(sst);

    return normalOrientationStatus;
#else
    return 0;
#endif
}

/* */
int TriangleSet::unitize(TinyMatrix *transform, BOOL forceRecalculation,
                         BOOL doNotScale)
{
    double      bbDiameter, scalef;
    TinyVector  com;
    TinyMatrix  transf, scale, transl;

    if(!doNotScale)
    {
        bbDiameter = this->getBoundingBoxDiameter(forceRecalculation);
        scalef = 1. / bbDiameter;
    }

    com = centerOfMass();
    transl.trans(-com[0], -com[1], -com[2]);
    if(!doNotScale)
    {
        scale.scale(scalef, scalef, scalef);
        transf = scale * transl;
    }
    else
        transf = transl;
    this->transform(&transf);
    if(transform)
        *transform = transf;

    return 0;
}

/* */
int TriangleSet::center(TinyMatrix *transform)
{
    TinyVector  com;
    TinyMatrix  transf, transl;

    com = centerOfMass();
    transl.trans(-com[0], -com[1], -com[2]);
    transf = transl;
    this->transform(&transf);
    if(transform)
        *transform = transf;
    return 0;
}

#define SCALEF  0.5

/* */

int TriangleSet::fitInUnitCube(TinyMatrix *transform, BOOL applyToMesh)
{
    TinyVector  llf, urb;
    TinyVector  v, com;
    TinyMatrix  transl1, transl2, scale;
    TinyVector  llf2, urb2;
    double      max, max2;
    double      scalef, scalef2;
    int         i, n;

    this->getBoundingBox(&llf, &urb);

    com = this->centerOfMass();
    transl1.trans(-com[0], -com[1], -com[2]);
    llf2 = transl1 * llf;
    urb2 = transl1 * urb;

    /* now do uniform scaling */
    max = MAX3(llf2[0], llf2[1], llf2[2]);
    if(fabs(max) < ZERO_TOL)
        return -1;
    scalef = SCALEF / fabs(max);

    max2 = MAX3(urb2[0], urb2[1], urb2[2]);
    if(fabs(max2) < ZERO_TOL)
        return -1;
    scalef2 = SCALEF / fabs(max2);

    scalef = MIN2(scalef, scalef2);

    //transl2.trans(SCALEF,SCALEF,SCALEF);
    scale.scale(scalef, scalef, scalef);

    *transform = scale * transl1;

    if(applyToMesh)
    {
        n = this->vertices();
        for(i = 0; i < n; i++)
        {
            v = this->getVertex(i);
            v = *transform * v;
            this->setVertex(i, v);
        }
    }

    return 0;
}

/* */
int TriangleSet::fitInUnitSphere(TinyMatrix *transform, BOOL applyToMesh)
{
    TinyVector  v, com;
    int         i, n;
    double      dist, maxDist = 0;
    double      scalef;
    TinyMatrix  trans, scale;

    com = this->centerOfMass();
    n = this->vertices();
    for(i = 0; i < n; i++)
    {
        v = this->getVertex(i);
        v = v - com;
        dist = tv_abs(v);
        if(dist > maxDist)
            maxDist = dist;
    }

    if(maxDist < ZERO_TOL)
        return -1;
    scalef = 1. / maxDist;
    trans.trans(-com[0], -com[1], -com[2]);
    scale.scale(scalef, scalef, scalef);
    *transform = scale * trans;

    if(applyToMesh)
    {
        for(i = 0; i < n; i++)
        {
            v = this->getVertex(i);
            v = *transform * v;
            this->setVertex(i, v);
        }
    }

    return 0;
}

/* */
int TriangleSet::transform(TinyMatrix *transform, BOOL inverse)
{
    TinyVector  v;
    int         i, n, rv;

    if(inverse)
    {
        rv = transform->inverse(*transform);
        if(rv != 0)
            return -1;
    }

    n = this->vertices();
    for(i = 0; i < n; i++)
    {
        v = this->getVertex(i);
        v = *transform * v;
        this->setVertex(i, v);
    }

    return 0;
}

/* */
int TriangleSet::getUnsharedVertex(int trId1, int trId2, int *shared)
{
    int vids1[3], vids2[3];
    int i, j, found, numNotFound = 0, numFound = 0;
    int idsNotFound[3];
    int idsFound[3];
    int a, b;

    getTriangleVertices(trId1, vids1);
    getTriangleVertices(trId2, vids2);

    for(i = 0; i < 3; i++)
    {
        j = 0;
        found = 0;
        while((j < 3) && (!found))
        {
            found = (vids2[i] == vids1[j]);
            if(found)
            {
                idsFound[numFound] = j;
                numFound++;
            }

            j++;
        }

        if(!found)
        {
            idsNotFound[numNotFound] = vids2[i];
            numNotFound++;
        }
    }

    if(numNotFound != 1)
        return -1;
    else
    {
        a = MIN2(idsFound[0], idsFound[1]);
        b = MAX2(idsFound[0], idsFound[1]);

        if((a == 0) && (b == 1))
            *shared = 0;
        else if((a == 1) && (b == 2))
            *shared = 1;
        else
            *shared = 2;

        return idsNotFound[0];
    }
}

/* */
int TriangleSet::getUnsharedVertex(int *vIds, int trId2, int *shared)
{
    int vids2[3];
    int i, j, found, numNotFound = 0, numFound = 0;
    int idsNotFound[3];
    int idsFound[3];
    int a, b;

    getTriangleVertices(trId2, vids2);

    for(i = 0; i < 3; i++)
    {
        j = 0;
        found = 0;
        while((j < 3) && (!found))
        {
            found = (vids2[i] == vIds[j]);
            if(found)
            {
                idsFound[numFound] = j;
                numFound++;
            }

            j++;
        }

        if(!found)
        {
            idsNotFound[numNotFound] = vids2[i];
            numNotFound++;
        }
    }

    if(numNotFound != 1)
        return -1;
    else
    {
        a = MIN2(idsFound[0], idsFound[1]);
        b = MAX2(idsFound[0], idsFound[1]);

        if((a == 0) && (b == 1))
            *shared = 0;
        else if((a == 1) && (b == 2))
            *shared = 1;
        else
            *shared = 2;

        return idsNotFound[0];
    }
}

/* */
int TriangleSet::numberOfSharedVertices(int trId1, int trId2)
{
    int vids1[3], vids2[3];
    int i, j, found, numFound = 0;

    getTriangleVertices(trId1, vids1);
    getTriangleVertices(trId2, vids2);

    for(i = 0; i < 3; i++)
    {
        j = 0;
        found = 0;
        while((j < 3) && (!found))
        {
            found = (vids2[i] == vids1[j]);
            if(found)
                numFound++;
            j++;
        }
    }

    return numFound;
}

/* */
int TriangleSet::getSharedVertices(int trId1, int trId2, int *vIds)
{
    int vids1[3], vids2[3];
    int i, j, found, numFound = 0;

    getTriangleVertices(trId1, vids1);
    getTriangleVertices(trId2, vids2);

    for(i = 0; i < 3; i++)
    {
        j = 0;
        found = 0;
        while((j < 3) && (!found))
        {
            found = (vids2[i] == vids1[j]);
            if(found)
            {
                vIds[numFound] = vids2[i];
                numFound++;
            }

            j++;
        }
    }

    return numFound;
}

/* */
int TriangleSet::getUnsharedVertices(int trId1, int trId2, int *vIds)
{
    int     vids1[3], vids2[3];
    int     i, j, numNotFound = 0;
    BOOL    found = FALSE;

    getTriangleVertices(trId1, vids1);
    getTriangleVertices(trId2, vids2);

    for(i = 0; i < 3; i++)
    {
        j = 0;
        found = FALSE;
        while((j < 3) && (!found))
        {
            found = (vids1[i] == vids2[j]);
            j++;
        }

        if(!found)
        {
            vIds[numNotFound] = vids1[i];
            numNotFound++;
        }
    }

    found = FALSE;
    for(i = 0; i < 3; i++)
    {
        j = 0;
        found = FALSE;
        while((j < 3) && (!found))
        {
            found = (vids2[i] == vids1[j]);
            j++;
        }

        if(!found)
        {
            vIds[numNotFound] = vids2[i];
            numNotFound++;
        }
    }

    return numNotFound;
}

/*
  triangle is degenerated if

  - two or three identical points 

  further possible checks (not performed):

  - check triangle face size (disregard small ones)
  - check edge lengths (disregard small ones -> 
    no normal calculation possible)
*/
BOOL TriangleSet::isDegenerated(int trId)
{
    int vIds[3];

    getTriangleVertices(trId, vIds);
    if((vIds[0] == vIds[1]) || (vIds[1] == vIds[2]) || (vIds[0] == vIds[2]))
        return TRUE;
    else
        return FALSE;
}

/* */
BOOL TriangleSet::hasDegeneratedShape(int trId, DegenSpec *degenSpec)
{
    int         i;
    double      lengths[3];
    double      ratio;
    int         vIds[3];
    TinyVector  v[3], v1, v2;
    double      angle;

    triangleEdgeLengths(trId, this, lengths);
    if((lengths[0] < degenSpec->edgeMinLength) ||
       (lengths[1] < degenSpec->edgeMinLength) ||
       (lengths[2] < degenSpec->edgeMinLength))
        return TRUE;

    ratio = lengths[0] / lengths[1];
    if((ratio < degenSpec->lolimit) || (ratio > degenSpec->uplimit))
        return TRUE;

    ratio = lengths[1] / lengths[2];
    if((ratio < degenSpec->lolimit) || (ratio > degenSpec->uplimit))
        return TRUE;

    ratio = lengths[0] / lengths[2];
    if((ratio < degenSpec->lolimit) || (ratio > degenSpec->uplimit))
        return TRUE;

    this->getTriangleVertices(trId, vIds);
    for(i = 0; i < 3; i++)
        v[i] = this->getVertex(vIds[i]);

    /* check angles in triangle */
    v1 = v[1] - v[0];
    v2 = v[2] - v[0];
    v1.normalize();
    v2.normalize();
    if(v1.angle(v2) < degenSpec->minAngle)
        return TRUE;
    v1 = v[0] - v[1];
    v2 = v[2] - v[1];
    v1.normalize();
    v2.normalize();
    if(v1.angle(v2) < degenSpec->minAngle)
        return TRUE;
    v1 = v[1] - v[2];
    v2 = v[0] - v[2];
    v1.normalize();
    v2.normalize();
    if(v1.angle(v2) < degenSpec->minAngle)
        return TRUE;

    return FALSE;
}

/* */
BOOL hasDegeneratedShape(TinyVector v1, TinyVector v2, TinyVector v3,
                         DegenSpec *degenSpec)
{
    int         i;
    double      lengths[3];
    double      ratio;
    int         vIds[3];
    TinyVector  a, b;
    double      angle;

    triangleEdgeLengths(v1, v2, v3, lengths);
    if((lengths[0] < degenSpec->edgeMinLength) ||
       (lengths[1] < degenSpec->edgeMinLength) ||
       (lengths[2] < degenSpec->edgeMinLength))
        return TRUE;

    ratio = lengths[0] / lengths[1];
    if((ratio < degenSpec->lolimit) || (ratio > degenSpec->uplimit))
        return TRUE;

    ratio = lengths[1] / lengths[2];
    if((ratio < degenSpec->lolimit) || (ratio > degenSpec->uplimit))
        return TRUE;

    ratio = lengths[0] / lengths[2];
    if((ratio < degenSpec->lolimit) || (ratio > degenSpec->uplimit))
        return TRUE;

    /* check angles in triangle */
    a = v2 - v1;
    b = v3 - v1;
    a.normalize();
    b.normalize();
    if(a.angle(b) < degenSpec->minAngle)
        return TRUE;
    a = v1 - v2;
    b = v3 - v2;
    a.normalize();
    b.normalize();
    if(a.angle(b) < degenSpec->minAngle)
        return TRUE;
    a = v2 - v3;
    b = v1 - v3;
    a.normalize();
    b.normalize();
    if(a.angle(b) < degenSpec->minAngle)
        return TRUE;

    return FALSE;
}

/* */
int TriangleSet::reportDegeneracies(DegenSpec *degenSpec,
                                    GEOMARK_BitArray *degeneratedFaces,
                                    GEOMARK_BitArray *degeneratedVertices,
                                    int *numDegenFaces, int *numDegenVertices)
{
    int     i, j, n, m, trId;
    BOOL    degeneratedAdjFace;
    int     numDegeneratedFaces, numDegeneratedVertices;

    numDegeneratedFaces = 0;
    numDegeneratedVertices = 0;
    n = nf_;
    for(i = 0; i < n; i++)
    {
        if(hasDegeneratedShape(i, degenSpec))
        {
            degeneratedFaces->setBit(i, 1);
            numDegeneratedFaces++;
        }
        else
            degeneratedFaces->setBit(i, 0);
    }

    if(degeneratedVertices)
    {
        n = np_;
        for(i = 0; i < n; i++)
        {
            getAdjacentTriangles(i, tmpTriangleList_[2]);
            m = tmpTriangleList_[2]->getNumElements();
            degeneratedAdjFace = FALSE;
            j = 0;
            while((j < m) && (!degeneratedAdjFace))
            {
                trId = *((int *) tmpTriangleList_[2]->getElement(j));
                if(degeneratedFaces->getBit(trId))
                    degeneratedAdjFace = TRUE;
                j++;
            }

            if(degeneratedAdjFace)
                numDegeneratedVertices++;
            degeneratedVertices->setBit(i, degeneratedAdjFace);
            tmpTriangleList_[2]->empty();
        }
    }

    *numDegenFaces = numDegeneratedFaces;
    *numDegenVertices = numDegeneratedVertices;

    return 0;
}

/* */
TinyVector TriangleSet::getVertex(int vertexId)
{
    return *points_[vertexId];
}

/* */
int TriangleSet::getTriangleVertex(int triId, int vertNo)
{
    return faces_[triId].ind[vertNo];
}

/* */
int TriangleSet::getTriangleVertices(int trId, int *vIds)
{
    vIds[0] = faces_[trId].ind[0];
    vIds[1] = faces_[trId].ind[1];
    vIds[2] = faces_[trId].ind[2];

    return 0;
}

/* */
void TriangleSet::setVertex(int vertexId, const TinyVector &v)
{
    *points_[vertexId] = v;
}

/* */
void TriangleSet::setFace(int triangleId, int i1, int i2, int i3,
                          BOOL updateHashTable)
{
    int             ind[3], h, rv, i;
    unsigned char   key[9];

    ind[0] = i1;
    ind[1] = i2;
    ind[2] = i3;

    faces_[triangleId].ind[0] = i1;
    faces_[triangleId].ind[1] = i2;
    faces_[triangleId].ind[2] = i3;

    //adjTriangles_[ind[0]]->appendElement(&triangleId);
    //adjTriangles_[ind[1]]->appendElement(&triangleId);
    //adjTriangles_[ind[2]]->appendElement(&triangleId);
    if(updateHashTable)
    {
        /* order vertex ids acending */
        if(ind[0] > ind[1])
        {
            h = ind[0];
            ind[0] = ind[1];
            ind[1] = h;
        }

        if(ind[1] > ind[2])
        {
            h = ind[1];
            ind[1] = ind[2];
            ind[2] = h;
        }

        if(ind[0] > ind[1])
        {
            h = ind[0];
            ind[0] = ind[1];
            ind[1] = h;
        }

        memcpy((void *) &key[0], (void *) &ind[0], 3);
        memcpy((void *) &key[3], (void *) &ind[1], 3);
        memcpy((void *) &key[6], (void *) &ind[2], 3);

        rv = triangleHashTable_->searchElement((void *) &key[0], NULL);
        if(rv)
        {
            /* new pattern */
            triangleHashTable_->insertElement((void *) &key[0],
                                              (void *) numPatterns_);
            numPatterns_++;
        }
    }                   /* if updateHashTable */
}

/* */
int TriangleSet::isEdgePartOfTriangle(int trId, int vId1, int vId2)
{
    int vIds[3];
    int n = 0;

    this->getTriangleVertices(trId, vIds);

    if((
           ((vId1 == vIds[0]) && (n += 1)) ||
       ((vId1 == vIds[1]) && (n += 2)) ||
       ((vId1 == vIds[2]) && (n += 4))
   ) &&
       (
           ((vId2 == vIds[0]) && (n += 1)) ||
       ((vId2 == vIds[1]) && (n += 2)) ||
       ((vId2 == vIds[2]) && (n += 4))
   ))
    {
        if((n != 5) && (n != 6) && (n != 3))
            return -2;  /* triangle degenerated */
        else
        {
            if(n == 3)
                return 2;
            else
                return 6 - n;

            /*
        first and third vertex of triangle on edge: 6-5=1 (second
          vertex of triangle is returned) 
      */
        }
    }

    return -1;
}

/* */
BOOL TriangleSet::isBorderVertex(int vertexId, int *numBorderVertices,
                                 int *borderVertexIds)
{
    int     i, j, n, m;
    int     trId, v1Edge, v2Edge;
    int     *vertexList, *toList;
    int     trHashId;
    int     bitVal;
    BOOL    liesOnBorder;
    int     numVerticesOnBorder;
    int     vm1, vm2;

    getAdjacentTriangles(vertexId, tmpTriangleList_[2]);
    n = tmpTriangleList_[2]->getNumElements();

    triangleHashMarked_->empty();

    for(i = 0; i < n; i++)
    {
        trId = *((int *) tmpTriangleList_[2]->getElement(i));
        trHashId = getTriangleHash(trId);

        if(!triangleHashMarked_->getBit(trHashId))
        {               /* remove double faces */
            getOppositeEdge(trId, vertexId, &v1Edge, &v2Edge);

            bitVal = verticesMarked_[0]->getBit(v1Edge);
            verticesMarked_[0]->setBit(v1Edge, !bitVal);    /* flip bit */
            bitVal = verticesMarked_[0]->getBit(v2Edge);
            verticesMarked_[0]->setBit(v2Edge, !bitVal);

            triangleHashMarked_->setBit(trHashId, 1);
        }
    }

    if(!numBorderVertices)
    {
        i = 0;
        liesOnBorder = FALSE;
        while((i < n) && (!liesOnBorder))
        {
            trId = *((int *) tmpTriangleList_[2]->getElement(i));
            getOppositeEdge(trId, vertexId, &v1Edge, &v2Edge);
            liesOnBorder =
                (
                    (verticesMarked_[0]->getBit(v1Edge)) ||
                    (verticesMarked_[0]->getBit(v2Edge))
                );
            i++;
        }
    }
    else
    {
        numVerticesOnBorder = 0;
        borderVertexIds[0] = -1;
        borderVertexIds[1] = -1;

        i = 0;
        liesOnBorder = FALSE;
        while(i < n)
        {
            trId = *((int *) tmpTriangleList_[2]->getElement(i));
            getOppositeEdge(trId, vertexId, &v1Edge, &v2Edge);
            vm1 = verticesMarked_[0]->getBit(v1Edge);
            vm2 = verticesMarked_[0]->getBit(v2Edge);
            if(!liesOnBorder)
                liesOnBorder = (vm1 || vm2);
            if(vm1)
            {
                if(numVerticesOnBorder < 2)
                {
                    if((numVerticesOnBorder == 0) ||
                       (borderVertexIds[0] != v1Edge))
                    {
                        borderVertexIds[numVerticesOnBorder] = v1Edge;
                    }
                }

                numVerticesOnBorder++;
            }

            if(vm2)
            {
                if(numVerticesOnBorder < 2)
                {
                    if((numVerticesOnBorder == 0) ||
                       (borderVertexIds[0] != v2Edge))
                    {
                        borderVertexIds[numVerticesOnBorder] = v2Edge;
                    }
                }

                numVerticesOnBorder++;
            }

            i++;
        }

        *numBorderVertices = numVerticesOnBorder;
    }

    tmpTriangleList_[2]->empty();
    verticesMarked_[0]->empty();

    return liesOnBorder;
}

/* */
BOOL TriangleSet::isEdge(int vId1, int vId2, double *length)
{
    BOOL    found;
    int     i, n;

    this->getAdjacentVertices(vId1, tmpVertexList_);
    n = tmpVertexList_->getNumElements();
    found = FALSE;
    i = 0;
    while((i < n) && (!found))
    {
        found = (vId2 == *((int *) tmpVertexList_->getElement(i)));
        i++;
    }

    tmpVertexList_->empty();
    *length = tv_abs(*points_[vId1] -*points_[vId2]);

    return found;
}

/* */
double TriangleSet::edgeLength(int vId1, int vId2)
{
    return tv_abs(*points_[vId1] -*points_[vId2]);
}

/* */
int TriangleSet::getOppositeEdge(int trId, int vId, int *vId1Edge, int *vId2Edge)
{
    int vIds[3];

    this->getTriangleVertices(trId, vIds);
    if(vIds[0] == vId)
    {
        if(vIds[1] < vIds[2])
        {
            *vId1Edge = vIds[1];
            *vId2Edge = vIds[2];
        }
        else
        {
            *vId1Edge = vIds[2];
            *vId2Edge = vIds[1];
        }
    }
    else if(vIds[1] == vId)
    {
        if(vIds[0] < vIds[2])
        {
            *vId1Edge = vIds[0];
            *vId2Edge = vIds[2];
        }
        else
        {
            *vId1Edge = vIds[2];
            *vId2Edge = vIds[0];
        }
    }
    else if(vIds[2] == vId)
    {
        if(vIds[0] < vIds[1])
        {
            *vId1Edge = vIds[0];
            *vId2Edge = vIds[1];
        }
        else
        {
            *vId1Edge = vIds[1];
            *vId2Edge = vIds[0];
        }
    }
    else
        return -1;

    return 0;
}

/*
  BE CAREFUL !!!

  returns pointer to precalculated GrowArray

  So don't delete it !

  *OBSOLETE*
*/
int TriangleSet::getAdjacentTriangles(int vertexId, GrowArray *adjTrianglesList)
{
    assert((vertexId >= 0) && (vertexId < np_) && (adjTrianglesList));

    if(!dataComplete_)
    {
        //sortTriangles_();
        generateAdjList_();
        dataComplete_ = TRUE;
        setNumAdjVertices_();
    }

    //getAdjTriangles_(vertexId,adjTrianglesList);
    getAdjTriangles2_(vertexId, adjTrianglesList);

    return 0;
}

/* a valid list pointer must be provided */
GrowArray *TriangleSet::getAdjacentVertices(int vertexId,
                                            GrowArray *adjVertexList)
{
    assert((vertexId >= 0) && (vertexId < np_));

    int         i, j, n, trId, vids[3], vid;

    //GEOMARK_BitArray copied(np_);
    GrowArray   *adjVertices;
    GrowArray   *adjTriangles;

    /* reuse of array */
    if(adjVertexList == NULL)
    {
        adjVertices = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(np_),
                                    sizeof(int));
    }
    else
        adjVertices = adjVertexList;

    adjTriangles = tmpTriangleList_[1];
    this->getAdjacentTriangles(vertexId, adjTriangles);

    n = adjTriangles->getNumElements();

    for(i = 0; i < n; i++)
    {
        trId = *((int *) adjTriangles->getElement(i));
        GET_TRIANGLE_VERTICES(trId, vids);

        //getTriangleVertices(trId,vids);
        for(j = 0; j < 3; j++)
        {
            vid = vids[j];

            if(vid != vertexId)
            {
                if(verticesCopied_->getBit(vid) < 1)
                {
                    verticesCopied_->setBit(vid, 1);
                    adjVertices->appendElement(&vid);
                }
            }
        }
    }

    tmpTriangleList_[1]->empty();
    verticesCopied_->empty();

    return adjVertices;
}

/* get indices of all triangles (vertex-) adjacent to one with id triangleId */
GrowArray *TriangleSet::getAdjacentTrianglesFromTriangle(int triangleId,
                                                         GrowArray *adjTriangleList,
                                                         int triangleSelection)
{
    if(adjTriangleList == NULL)
    {
        adjTriangleList = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(MAX_NUM_FACES),
                                            sizeof(int));
    }

    int         i, n;
    GrowArray   *l[3];
    int         ne[3], ic[3];
    int         numEmpty;
    BOOL        empty[3];
    int         min, trId, minIndex, nonEmpty, numberOfElements;
    int         hashValue;
    int         vIds[3];

    /* BUGFIX: merging of sorted lists was a mess ... */
    numEmpty = 0;
    for(i = 0; i < 3; i++)
    {
        l[i] = tmpTriangleList_[i + 1];
        this->getAdjacentTriangles(this->getTriangleVertex(triangleId, i), l[i]);

        //dumpGrowArrayInt(l[i]);
        ne[i] = l[i]->getNumElements();
        ic[i] = 0;
        if(ne[i] > 0)
            empty[i] = FALSE;
        else
        {
            empty[i] = TRUE;
            numEmpty++;
        }
    }

    /* getAdjacentTriangles gives sorted lists, so we can easily
    remove duplicates through merging */
    triangleHashMarked_->empty();

    getTriangleVertices(triangleId, vIds);
    hashValue = getTriangleHash(triangleId);
    triangleHashMarked_->setBit(hashValue, 1);

    while(numEmpty < 2)
    {
        min = -1;
        for(i = 0; i < 3; i++)
        {
            if(!empty[i])
            {
                trId = *((int *) l[i]->getElement(ic[i]));

                if((trId < min) || (min == -1))
                {
                    min = trId;
                    minIndex = i;
                }
                else if((trId == min) || (trId == triangleId))
                {       /* removing duplicates */
                    ic[i]++;
                    if(ic[i] >= ne[i])
                    {
                        numEmpty++;
                        empty[i] = TRUE;
                    }
                }
            }
        }

        ic[minIndex]++;

        /* BUGFIX */
        if(min != triangleId)
        {
            hashValue = getTriangleHash(min);

            if((
                   (!(triangleSelection & TRIANGLE_SEL_NO_DOUBLE_TRIDS)) ||
               (!triangleHashMarked_->getBit(hashValue))
           ) &&
               (
                   (!(triangleSelection & TRIANGLE_SEL_NO_DEGEN)) ||
               (!isDegenerated(min))
           ))
            {
                adjTriangleList->appendElement(&min);
                triangleHashMarked_->setBit(hashValue, 1);
            }
        }

        if(ic[minIndex] >= ne[minIndex])
        {
            numEmpty++;
            empty[minIndex] = TRUE;
        }
    }

    if(numEmpty == 2)
    {
        /* now simply copy the remaining elements */
        /* BUGFIX */
        nonEmpty = 0;
        while((nonEmpty < 3) && (empty[nonEmpty]))
            nonEmpty++;

        /* BUGFIX */
        //nonEmpty--;
        numberOfElements = ne[nonEmpty];
        i = ic[nonEmpty];
        while(i < numberOfElements)
        {
            trId = *((int *) l[nonEmpty]->getElement(i));

            if(trId != triangleId)
            {
                hashValue = getTriangleHash(trId);

                if((
                       (!(triangleSelection & TRIANGLE_SEL_NO_DOUBLE_TRIDS)) ||
                   (!triangleHashMarked_->getBit(hashValue))
               ) &&
                   (
                       (!(triangleSelection & TRIANGLE_SEL_NO_DEGEN)) ||
                   (!isDegenerated(trId))
               ))
                {
                    adjTriangleList->appendElement(&trId);
                    triangleHashMarked_->setBit(hashValue, 1);
                }
            }

            i++;
        }
    }                   /* if numEmpty==2 */

    n = adjTriangleList->getNumElements();

    for(i = 1; i < 4; i++)
        tmpTriangleList_[i]->empty();

    return adjTriangleList;
}

/* get indices of all triangles (edge-)adjacent to one with id triangleId */
GrowArray *TriangleSet::getEdgeAdjacentTriangles(int triangleId,
                                                 GrowArray *lresult,
                                                 int triangleSelection)
{
    int         i, j, k, n, m, *id;
    int         ids[3], cids[3], numSharedVertices;
    GrowArray   tmpa(POWER_CHUNK_SIZE_MAX_ELEMENTS(MAX_NUM_FACES), sizeof(int));

    if(lresult == NULL)
    {
        lresult = this->getAdjacentTrianglesFromTriangle(triangleId, NULL,
                                                         triangleSelection);
    }

    n = lresult->getNumElements();

    if(n > 0)
    {
        this->getTriangleVertices(triangleId, ids);

        m = 0;
        while(m < n)
        {
            id = (int *) lresult->getElement(m);
            this->getTriangleVertices(*id, cids);

            /* check if triangle is edge adjacent */
            numSharedVertices = 0;
            for(i = 0; i < 3; i++)
            {
                for(j = 0; j < 3; j++)
                {
                    if(ids[i] == cids[j])
                        numSharedVertices++;
                }
            }

            if(numSharedVertices < 2)
            {
                //printf("deleted!\n");
            }
            else
                tmpa.appendElement(id);

            m++;
        }

        lresult->empty();

#if 0
        /* dump tmpa */
        n = tmpa.getNumElements();
        for(i = 0; i < n; i++)
        {
            printf("%d\n", *((int *) tmpa.getElement(i)));
        }
#endif

        /* copy tmpa to lresult */
        lresult->copy(&tmpa);
    }                   /* if (n>0) */

    return lresult;
}

/* */
TinyVector TriangleSet::triangleNormal(int triangleId)
{
    TinyVector  normal, v0, v1, v2;

    //v0 = this->getVertex (mesh->getTriangleVertex (triangleId, 0));
    //v1 = this->getVertex (mesh->getTriangleVertex (triangleId, 1));
    //v2 = this->getVertex (mesh->getTriangleVertex (triangleId, 2));
    v0 = *(points_[faces_[triangleId].ind[0]]);
    v1 = *(points_[faces_[triangleId].ind[1]]);
    v2 = *(points_[faces_[triangleId].ind[2]]);

    //v0.dump();v1.dump();v2.dump();
    normal = (v1 - v0) * (v2 - v0);

    //normal.dump();
    normal.normalize(); /* if length of vector is <= ZERO_TOL
                         normalize() returns zero-vector ! */
    return normal;
}

/* */
int TriangleSet::vertexNormal(int vertexId, TinyVector *normal)
{
    int         trId, i, n, numNormals;
    TinyVector  tmpNormal, avgNormal;

    numNormals = 0;
    avgNormal = TinyVector(0, 0, 0);
    getAdjacentTriangles(vertexId, tmpTriangleList_[1]);
    n = tmpTriangleList_[1]->getNumElements();
    for(i = 0; i < n; i++)
    {
        trId = *((int *) tmpTriangleList_[1]->getElement(i));
        tmpNormal = triangleNormal(trId);

        /* if normal could be calculated */
        if((tmpNormal[0] != 0) || (tmpNormal[1] != 0) || (tmpNormal[2] != 0))
        {
            avgNormal = avgNormal + tmpNormal;
            numNormals++;
        }
    }

    if(numNormals > 0)
    {
        *normal = avgNormal / ((double) numNormals);

        //*normal=avgNormal;
        normal->normalize();
    }

    tmpTriangleList_[1]->empty();

    return numNormals;
}

/* returns > 0 in case of success */
int TriangleSet::vertexNormalWeightedByFaceSizes(int vertexId,
                                                 TinyVector *normal,
                                                 DegenSpec *spec)
{
    int         i, n, numNormals, trId;
    TinyVector  tmpNormal, avgNormal;
    double      fsize, sumFaceSizes;

    numNormals = 0;
    sumFaceSizes = 0;
    avgNormal.zero();
    getAdjacentTriangles(vertexId, tmpTriangleList_[1]);
    n = tmpTriangleList_[1]->getNumElements();
    for(i = 0; i < n; i++)
    {
        trId = *((int *) tmpTriangleList_[1]->getElement(i));
        if((!isDegenerated(i)) && (!hasDegeneratedShape(i, spec)))
        {
            tmpNormal = triangleNormal(trId);

            /* if normal could be calculated */
            if((tmpNormal[0] != 0) ||
               (tmpNormal[1] != 0) ||
               (tmpNormal[2] != 0))
            {
                fsize = this->faceSize(i);
                sumFaceSizes += fsize;
                avgNormal = avgNormal + (tmpNormal * fsize);
                numNormals++;
            }
        }
    }                   /* for i */

    if(numNormals > 0)
    {
        *normal = avgNormal / ((double) sumFaceSizes);  // sumFaceSizes should

        // be != 0
        normal->normalize();
    }
    else
        normal->zero();

    tmpTriangleList_[1]->empty();

    return numNormals;
}

/* */
int TriangleSet::triangleNormals(GrowArray **normals,
                                 GrowBitArray **degeneratedTriangles,
                                 DegenSpec *degenSpec)
{
    int             i, j, n, m;
    TinyVector      normal;
    int             vIds[3];
    GrowArray       *na;
    GrowBitArray    *dta;
    BOOL            degenerated;
    int             degenerateCount;
    TinyVector      v[3];
    double          edgeLength[3];
    TinyVector      edgeDir;

    /* the sizes below depend on MAX_NUM_CHUNKS and
    MAX_NUM_CHUNKS_GROWBITARRAY */
    if(*normals == NULL)
    {
        na = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(MAX_NUM_FACES),
                           sizeof(TinyVector));
    }
    else
    {
        (*normals)->empty();
        na = *normals;
    }

    if(*degeneratedTriangles == NULL)
    {
        dta = new GrowBitArray(POWER_CHUNK_SIZE_MAX_BITS(MAX_NUM_FACES));
    }
    else
    {
        (*degeneratedTriangles)->empty();
        dta = *degeneratedTriangles;
    }

    n = this->triangles();

    degenerateCount = 0;
    for(i = 0; i < n; i++)
    {
        /* DEGENERACY CHECK 

      ignore triangles consisting of 
        - three points on a line
        - two or three identical points 
        - a (normalized) normal that could not be calculated 
          (reason: too small edge lengths)

      FURTHERMORE:

	    - an edge must have minimum length to build a vector from
		  it (). we require this in lotPointLine().
	*/
        degenerated = FALSE;
        getTriangleVertices(i, vIds);

        if((vIds[0] == vIds[1]) ||
           (vIds[1] == vIds[2]) ||
           (vIds[0] == vIds[2]))
        {
            degenerated = TRUE;
        }

        /* if three points are on a line, a zero normal vector is 
      returned by triangleNormal().
     */
        normal = this->triangleNormal(i);

        //normal.dump();
        //if (i==27660) {
        //  printf("DEGEN CHECK\n");
        //  normal.dump();
        //}
        if((!degenerated) &&
           ((normal[0] == 0) && (normal[1] == 0) && (normal[2] == 0)))
        {
            degenerated = TRUE;
        }

#if 0
        if(!degenerated)
        {
            /* no sqrt calculation */
            edgeDir = *points_[vIds[0]] -*points_[vIds[1]];
            edgeLength[0] = TINYVECTOR_FAST_ABS(edgeDir);
            edgeDir = *points_[vIds[0]] -*points_[vIds[2]];
            edgeLength[1] = TINYVECTOR_FAST_ABS(edgeDir);
            edgeDir = *points_[vIds[1]] -*points_[vIds[2]];
            edgeLength[2] = TINYVECTOR_FAST_ABS(edgeDir);
            degenerated =
                (
                    (edgeLength[0] <= ZERO_TOL_EDGE_LENGTH) ||
                    (edgeLength[1] <= ZERO_TOL_EDGE_LENGTH) ||
                    (edgeLength[2] <= ZERO_TOL_EDGE_LENGTH)
                );
        }
#endif

        /* TODO: more extensive checking for degeneracies */
        if((!degenerated) && (degenSpec != NULL))
        {
            if(this->hasDegeneratedShape(i, degenSpec))
                degenerated = TRUE;
        }

        if(degenerated)
            degenerateCount++;

        dta->setBit(i, degenerated);
        na->appendElement(&normal);
    }

    *normals = na;
    *degeneratedTriangles = dta;

    return n;
}

/* */
int TriangleSet::triangleNormals(TinyVector *normals,
                                 GEOMARK_BitArray *degeneratedTriangles,
                                 DegenSpec *degenSpec)
{
    int         i, j, n, m;
    TinyVector  normal;
    int         vIds[3];
    BOOL        degenerated;
    int         degenerateCount;
    TinyVector  v[3];
    double      edgeLength[3];
    TinyVector  edgeDir;

    /* the sizes below depend on MAX_NUM_CHUNKS and
    MAX_NUM_CHUNKS_GROWBITARRAY */
    n = this->triangles();

    degenerateCount = 0;
    for(i = 0; i < n; i++)
    {
        /* DEGENERACY CHECK 

      ignore triangles consisting of 
        - three points on a line
        - two or three identical points 
        - a (normalized) normal that could not be calculated 
          (reason: too small edge lengths)

      FURTHERMORE:

	    - an edge must have minimum length to build a vector from
		  it (). we require this in lotPointLine().
	*/
        degenerated = FALSE;
        getTriangleVertices(i, vIds);

        if((vIds[0] == vIds[1]) ||
           (vIds[1] == vIds[2]) ||
           (vIds[0] == vIds[2]))
        {
            degenerated = TRUE;
        }

        /* if three points are on a line, a zero normal vector is 
      returned by triangleNormal().
     */
        normal = this->triangleNormal(i);

        //if (i==27660) {
        //  printf("DEGEN CHECK\n");
        //  normal.dump();
        //}
        if((!degenerated) &&
           ((normal[0] == 0) && (normal[1] == 0) && (normal[2] == 0)))
        {
            degenerated = TRUE;

            //printf("DEGEN 2 %d\n",i);
        }

#if 0
        if(!degenerated)
        {
            /* no sqrt calculation */
            edgeDir = *points_[vIds[0]] -*points_[vIds[1]];
            edgeLength[0] = TINYVECTOR_FAST_ABS(edgeDir);
            edgeDir = *points_[vIds[0]] -*points_[vIds[2]];
            edgeLength[1] = TINYVECTOR_FAST_ABS(edgeDir);
            edgeDir = *points_[vIds[1]] -*points_[vIds[2]];
            edgeLength[2] = TINYVECTOR_FAST_ABS(edgeDir);
            degenerated =
                (
                    (edgeLength[0] <= ZERO_TOL_EDGE_LENGTH) ||
                    (edgeLength[1] <= ZERO_TOL_EDGE_LENGTH) ||
                    (edgeLength[2] <= ZERO_TOL_EDGE_LENGTH)
                );
        }
#endif

        /* TODO: more extensive checking for degeneracies */
        if((!degenerated) && (degenSpec != NULL))
        {
            if(this->hasDegeneratedShape(i, degenSpec))
                degenerated = TRUE;
        }

        if(degenerated)
            degenerateCount++;

        degeneratedTriangles->setBit(i, degenerated);
        normals[i] = normal;
    }   /* for i */

    //printf("DEGEN-COUNT=%d\n",degenerateCount);
    //getchar();
    return degenerateCount;
}

/* */
int TriangleSet::faceSizes(GrowArray **sizes)
{
    int             i, n;
    GrowArray       *sa;
    double          fs;
    TriangleFace    face;
    TinyVector      a, b, c;

    n = this->triangles();

    if(*sizes == NULL)
    {
        sa = new GrowArray(POWER_CHUNK_SIZE_MAX_ELEMENTS(MAX_NUM_FACES),
                           sizeof(double));
    }
    else
    {
        (*sizes)->empty();
        sa = *sizes;
    }

    for(i = 0; i < n; i++)
    {
        a = *points_[faces_[i].ind[0]];
        b = *points_[faces_[i].ind[1]];
        c = *points_[faces_[i].ind[2]];

        fs = tv_abs((b - a) * (c - a)) / 2.;

        sa->appendElement(&fs);
    }

    return 0;
}

/* */
double TriangleSet::faceSize(int triangleId)
{
    double      fs;
    TinyVector  a, b, c;

    a = *points_[faces_[triangleId].ind[0]];
    b = *points_[faces_[triangleId].ind[1]];
    c = *points_[faces_[triangleId].ind[2]];

    fs = tv_abs((b - a) * (c - a)) / 2.;

    return fs;
}

/* */
TinyVector TriangleSet::centerOfMass(void)
{
    int         i, n;
    TinyVector  v;

    n = vertices();
    if(n > 0)
    {
        for(i = 0; i < n; i++)
            v = v + getVertex(i);
        return TinyVector(v[0] / (double) n, v[1] / (double) n, v[2] / (double) n);
    }
    else
        return TinyVector();
}
#endif
