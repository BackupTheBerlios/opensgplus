#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
/*#ifdef OSG_STREAM_IN_STD_NAMESPACE
#include <iostream>
#include <fstream>
#else
#include <iostream.h>
#include <fstream.h>
#endif
*/
//#include <GL/glut.h>
#include <OSGConfig.h>
#include <OSGSimpleGeometry.h>
//#include <OSGGLUTWindow.h>
#include <OSGSimpleSceneManager.h>
#include <OSGAction.h>

#include <OSGVRMLWriteAction.h>
#include <OSGVRMLFile.h>

// the general scene file loading handler
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

// New Headers in OpenSG ver. 1.2, WF 03.07.2003
#include <OSGAttachment.h>
#include <OSGSimpleAttachments.h>

/* geomark stuff */
#include "OSGWMflood2.h"
#include "OSGWMtriangleset.h"

OSG_USING_NAMESPACE
#define WM_ACTION_NONE              0
#define WM_ACTION_EMBED             1
#define WM_ACTION_RETRIEVE          2

#define WATERMARK_TYPE_VFLOOD       2
#define MAX_LENGTH_WM_DESCRIPTION   256

#include "OSGWMSceneGraphAccess.h"

/*
  Parsen des Szenengraphen und sammeln von IndexedFaceSet-
  bezogener Information.

  Fuer jeden Knoten im Szenengraphen wird dabei die Funktion
  Action::ResultE enter(NodePtr& node) [siehe unten]
  ausgefuehrt.
  Als Ergebniss kann anschliessend auf die IndexedFaceSet-Geometrie
  ueber die member variable meshInfo_ zugegriffen werden.
*/
WmSceneGraphAccess * gl_sga;

/* */

WmSceneGraphAccess::WmSceneGraphAccess(NodePtr *pScene)
{
    int i;
    int meshId;

    /* parse scenegraph */
    gl_sga = this;  // MS: Argh!
    numGeometryNodes_ = 0;
    nodeDepth_ = 0;
    maxNodeDepth_ = -1;
    numMeshes_ = 0;
    meshId = 0;

    /* OpenSG-vorgegebene Traversierungsfunktion */
    traverse(*pScene, osgTypedFunctionFunctor1CPtrRef < Action::ResultE,
             NodePtr > (enter),
             osgTypedFunctionFunctor2CPtrRef < Action::ResultE, NodePtr,
             Action::ResultE > (leave));

    SINFO <<
        "RESULT:\n" <<
        "\tnumGeometryNodes_=" <<
        numGeometryNodes_ <<
        endl <<
        "\tmaxNodeDepth_=" <<
        maxNodeDepth_ <<
        endLog;

    /* sort meshInfo-entries based on vPtr */
    if(numGeometryNodes_ > 0)
    {
        numMeshes_++;
        sort(meshInfo_.begin(), meshInfo_.end());
        for(i = 0; i < numGeometryNodes_; i++)
        {
            if((i > 0) && (meshInfo_[i].vPtr != meshInfo_[i - 1].vPtr))
            {
                meshId++;
                numMeshes_++;
            }

            meshInfo_[i].meshId = meshId;
            meshInfo_[i].dump();
        }
    }

    SINFO << "  numMeshes_=" << numMeshes_ << endLog;
}

/* */
WmSceneGraphAccess::~WmSceneGraphAccess(void)
{ }

/*
  Konvertierung des mit meshId bezeichneten IndexedFaceSets in ein Mesh
  vom Typ TriangleSet.
  Zu beachten:
  IndexedFaceSets, die Eckpunkte "shared", werden beim Parsen des
  Szenengraphen (Konstruktor) zu einem Teilmesh zusammengefasst
  (erhalten dieselbe ID).
  Bei der Konvertierung wird aus diesen ein Mesh generiert.
*/
int WmSceneGraphAccess::convertToTs(TriangleSet *ts, int *numQuadsFound,
                                    GEOMARK_BitArray **isQuadVertex, int meshId)
{
    int             i, j, k;
    int             offsetVertices, numVertices, numFaces, numFaceVertices;
    int             to, from;
    TinyVector      v;
    FaceIterator    fit;
    int             trVIds[3], quadVIds[4];
    GeometryPtr     geoPtr;
    BOOL            quadsPresent;
    int             numQuads;

    if(numMeshes_ < 1)
        return -1;

    offsetVertices = 0;
    *isQuadVertex = NULL;
    quadsPresent = FALSE;
    numQuads = 0;

    if(meshId == -1)
    {               /* generate single flat mesh */
        /* first pass */
        SINFO << "Generating single flat mesh" << endl << endLog;

        numVertices = 0;
        numFaces = 0;
        numQuads = 0;
        for(i = 0; i < numGeometryNodes_; i++)
        {
            if((i == 0) || (meshInfo_[i].meshId != meshInfo_[i - 1].meshId))
            {
                numVertices += meshInfo_[i].numVertices;
                SINFO <<
                    "mesh id = " <<
                    meshInfo_[i].meshId <<
                    endl <<
                    "# of vertices = " <<
                    meshInfo_[i].numVertices <<
                    endl <<
                    endLog;
            }

            numFaces += meshInfo_[i].numFaces;
            numQuads += meshInfo_[i].numQuads;
            if((!quadsPresent) && (meshInfo_[i].numQuads > 0))
                quadsPresent = TRUE;
        }

        if(quadsPresent)
        {
            *isQuadVertex = new GEOMARK_BitArray(numVertices);
            numFaces += numQuads;
        }

        SINFO << "# of faces = " << numFaces << endl << endLog;

        ts->init(numVertices, numFaces);

        /* second pass */
        numVertices = 0;
        numFaces = 0;

        for(i = 0; i < numGeometryNodes_; i++)
        {
            if((i > 0) && (meshInfo_[i].meshId != meshInfo_[i - 1].meshId))
            {
                offsetVertices += meshInfo_[i - 1].numVertices;
            }

            if((i == 0) || (meshInfo_[i].meshId != meshInfo_[i - 1].meshId))
            {
                /* set vertex data */
                for(j = 0; j < meshInfo_[i].vPtr->size(); j++)
                {
                    v[0] = (*meshInfo_[i].vPtr)[j][0];
                    v[1] = (*meshInfo_[i].vPtr)[j][1];
                    v[2] = (*meshInfo_[i].vPtr)[j][2];

                    /* transformation does matter for case of single flat mesh */
                    v = meshInfo_[i].transf * v;    /* transform into world space */

                    ts->setVertex(offsetVertices + j, v);
                }

                numVertices += meshInfo_[i].numVertices;
            }

            /* set face data */
            geoPtr = meshInfo_[i].geoPtr;

            for(fit = geoPtr->beginFaces(); fit != geoPtr->endFaces(); ++fit)
            {
                /* this should be always 3 */
                numFaceVertices = fit.getLength();
                if(numFaceVertices == 4)
                {
                    for(j = 0; j < 4; j++)
                    {
                        quadVIds[j] = fit.getPositionIndex(j);
                        (*isQuadVertex)->setBit(quadVIds[j], 1);
                    }

                    /* split quad into two triangles */
                    ts->setFace(numFaces, offsetVertices + quadVIds[0],
                                offsetVertices + quadVIds[1],
                                offsetVertices + quadVIds[2]);
                    numFaces++;
                    ts->setFace(numFaces, offsetVertices + quadVIds[2],
                                offsetVertices + quadVIds[3],
                                offsetVertices + quadVIds[0]);
                    numFaces++;
                }
                else
                {
                    if(numFaceVertices != 3)
                    {
                        printf("numFaceVertices=%d\n", numFaceVertices);

                        // exit(1);
                        return -1;
                    }

                    for(j = 0; j < 3; j++)
                    {
                        trVIds[j] = fit.getPositionIndex(j);
                    }

                    ts->setFace(numFaces, offsetVertices + trVIds[0],
                                offsetVertices + trVIds[1],
                                offsetVertices + trVIds[2]);
                    numFaces++;
                }
            }                               /* for fit */
        }                                   /* for i */

        ts->meshComplete();
    }
    else
    {
        /* generate triangleset for mesh meshId */
        from = 0;

        while((from < numGeometryNodes_) && (meshInfo_[from].meshId < meshId))
        {
            from++;
        }

        if((from < numGeometryNodes_) && (meshInfo_[from].meshId == meshId))
        {
            /* first pass: count vertices and faces  */
            numVertices = 0;
            numFaces = 0;
            offsetVertices = 0;             /* const 0 */
            i = from;
            numVertices = meshInfo_[i].numVertices;
            numQuads = 0;
            while((i < numGeometryNodes_) && (meshInfo_[i].meshId == meshId))
            {
                numFaces += meshInfo_[i].numFaces;
                numQuads += meshInfo_[i].numQuads;
                i++;
            }

            SINFO << "# of vertices = " << numVertices << endLog;
            SINFO << "# of quads = " << numQuads << endLog;
            SINFO << "# of faces = " << numFaces << endl << endLog;

            if(numQuads > 0)
            {
                *isQuadVertex = new GEOMARK_BitArray(numVertices);
                numFaces += numQuads;
            }

            ts->init(numVertices, numFaces);

            /* second pass */
            numVertices = 0;
            numFaces = 0;
            offsetVertices = 0;
            i = from;
            while((i < numGeometryNodes_) && (meshInfo_[i].meshId == meshId))
            {
                /* set vertex data */
                for(j = 0; j < meshInfo_[i].vPtr->size(); j++)
                {
                    v[0] = (*meshInfo_[i].vPtr)[j][0];
                    v[1] = (*meshInfo_[i].vPtr)[j][1];
                    v[2] = (*meshInfo_[i].vPtr)[j][2];

                    /* transformation does not matter for single IndexedFaceSets */
                    ts->setVertex(offsetVertices + j, v);
                }

                /* set face data */
                geoPtr = meshInfo_[i].geoPtr;

                for(fit = geoPtr->beginFaces(); fit != geoPtr->endFaces();
                    ++fit)
                {
                    numFaceVertices = fit.getLength();
                    if(numFaceVertices == 4)
                    {
                        SWARNING <<
                            "Quad found. Will be split" <<
                            endl <<
                            endLog;
                        for(j = 0; j < 4; j++)
                        {
                            quadVIds[j] = fit.getPositionIndex(j);
                            (*isQuadVertex)->setBit(quadVIds[j], 1);
                        }

                        /* split quad into two triangles */
                        ts->setFace(numFaces, offsetVertices + quadVIds[0],
                                    offsetVertices + quadVIds[1],
                                    offsetVertices + quadVIds[2]);
                        numFaces++;
                        ts->setFace(numFaces, offsetVertices + quadVIds[2],
                                    offsetVertices + quadVIds[3],
                                    offsetVertices + quadVIds[0]);
                        numFaces++;
                    }
                    else
                    {
                        if(numFaceVertices != 3)
                        {
                            SFATAL <<
                                "# of vertices for face = " <<
                                numFaceVertices <<
                                endl <<
                                endLog;
                            return 1;
                        }

                        for(j = 0; j < 3; j++)
                        {
                            trVIds[j] = fit.getPositionIndex(j);
                        }

                        ts->setFace(numFaces, offsetVertices + trVIds[0],
                                    offsetVertices + trVIds[1],
                                    offsetVertices + trVIds[2]);
                        numFaces++;
                    }
                }                           /* for fit */

                i++;
            }                               /* while i */

            SINFO << "# of faces = " << numFaces << endl << endLog;

            ts->meshComplete();
        }                                   /* if */
        else
        {
            SFATAL << "No mesh with id " << meshId << endl << endLog;
            return 1;
        }
    }                                       /* generate flat mesh from all nodes */

    *numQuadsFound = numQuads;

    return 0;
}

/*
  Zurückschreiben eines modifizierten Meshs in den Szenengraphen.
  Die mittels der Mesh-ID referenzierte Geometrie wird
  aktualisiert.

  Wichtiger Hinweis: Es sind keine Topologie-Aenderungen
  realisiert. Das einzige was aktualisiert wird sind
  Eckpunktkoordinaten.
*/
int WmSceneGraphAccess::writeBack(TriangleSet *ts, int meshId)
{
    int         i, j;
    int         offsetVertices;
    TinyVector  v;
    int         numVertices, numVerticesWritten;
    int         from;
    TinyMatrix  itransf;
    int         mId;

    if(numMeshes_ < 1)
        return -1;

    if(meshId == -1)
    {
        /* propagate changes from single flat mesh to individual
	  IndexedFaceSets */
        offsetVertices = 0;
        numVerticesWritten = 0;

        mId = meshInfo_[0].meshId;

        i = 0;
        while(i < numGeometryNodes_)
        {
            if((i > 0) && (meshInfo_[i].meshId != meshInfo_[i - 1].meshId))
            {
                offsetVertices += meshInfo_[i - 1].numVertices;
            }

            meshInfo_[i].transf.inverse(itransf);

            for(j = 0; j < meshInfo_[i].numVertices; j++)
            {
                v = ts->getVertex(numVerticesWritten);

                //v=TinyVector(1,0,0);
                /* apply inverse transformation */
                v = itransf * v;
                (*meshInfo_[i].vPtr)[j][0] = v[0];
                (*meshInfo_[i].vPtr)[j][1] = v[1];
                (*meshInfo_[i].vPtr)[j][2] = v[2];
                numVerticesWritten++;
            }

            while((i < numGeometryNodes_) && (meshInfo_[i].meshId == mId))
                i++;
            if(i < numGeometryNodes_)
                mId = meshInfo_[i].meshId;
        }                                   /* while i */
    }
    else
    {
        from = 0;
        while((from < numGeometryNodes_) && (meshInfo_[from].meshId < meshId))
        {
            from++;
        }

        if((from < numGeometryNodes_) && (meshInfo_[from].meshId == meshId))
        {
            numVertices = ts->vertices();
            for(i = 0; i < numVertices; i++)
            {
                v = ts->getVertex(i);

                /* the coordinates are alread localy (object) coordinates */
                (*meshInfo_[from].vPtr)[i][0] = v[0];
                (*meshInfo_[from].vPtr)[i][1] = v[1];
                (*meshInfo_[from].vPtr)[i][2] = v[2];
            }
        }
    }

    return 0;
}

/*
  Quads und allgemein Polygone mit mehr als 3 Eckpunkten stellen ein
  besonderes Problem dar:

  Zum Einen ermoeglichen die Geomark-Routinen nur eine Verarbeitung von
  Dreiecksflaechen, zum anderen kann durch eine Aktualisierung
  ganz allgemein die Planaritaet verletzt werden.

  Wir umgehen diese Probleme, indem wir Quads und Polygone aus einem
  Mesh herausfiltern, genauer:
  - wir entfernen die Eckpunkte, die Bestandteile von Quads/Polygonen sind
  - wir entfernen Flachen, die zu Eckpunkten von Quads/Polygonen adjazent
    sind

  In vertexMap halten wir fest, welcher Eckpunkt der gefilterten Meshs
  welchem Eckpunkt im Original-Mesh entspricht:

  vertex[<filtered-mesh-ID>]=<orig-mesh-ID>

  Das gefilterte Mesh wird in outTs zurückgeliefert.

  Speicher fuer outTs und vertexMap wird in der Funktion allokiert.
*/
int WmSceneGraphAccess::filterOutQuads(TriangleSet *inTs, TriangleSet **outTs,
                                       GEOMARK_BitArray *isQuadVertex,
                                       int **vertexMap, double *bbDiameter)
{
    int         i, nv, nf;
    int         numQuadVertices, numQuadFaces;
    int         numVertices, numFaces;
    int         trVIds[3];
    TinyVector  v;
    int         *vertexMap2;

    /* pass 1 */
    *bbDiameter = inTs->getBoundingBoxDiameter();
    nv = inTs->vertices();
    nf = inTs->triangles();
    numQuadVertices = 0;
    numQuadFaces = 0;
    for(i = 0; i < nv; i++)
    {
        if(isQuadVertex->getBit(i))
            numQuadVertices++;
    }

    for(i = 0; i < nf; i++)
    {
        inTs->getTriangleVertices(i, trVIds);
        if((isQuadVertex->getBit(trVIds[0])) ||
           (isQuadVertex->getBit(trVIds[1])) ||
           (isQuadVertex->getBit(trVIds[2])))
        {
            numQuadFaces++;
        }
    }

    /* pass 2 */
    (*vertexMap) = (int *) malloc((nv - numQuadVertices) * sizeof(int));
    vertexMap2 = (int *) malloc(nv * sizeof(int));

    *outTs = new TriangleSet();
    (*outTs)->init(nv - numQuadVertices, nf - numQuadFaces);
    numVertices = 0;
    numFaces = 0;
    for(i = 0; i < nv; i++)
    {
        if(!isQuadVertex->getBit(i))
        {
            v = inTs->getVertex(i);
            (*outTs)->setVertex(numVertices, v);
            (*vertexMap)[numVertices] = i;
            vertexMap2[i] = numVertices;
            numVertices++;
        }
        else
        {
            vertexMap2[i] = -1;
        }
    }

    for(i = 0; i < nf; i++)
    {
        inTs->getTriangleVertices(i, trVIds);
        if((!isQuadVertex->getBit(trVIds[0])) &&
           (!isQuadVertex->getBit(trVIds[1])) &&
           (!isQuadVertex->getBit(trVIds[2])))
        {
            (*outTs)->setFace(numFaces, vertexMap2[trVIds[0]],
                              vertexMap2[trVIds[1]], vertexMap2[trVIds[2]]);
            numFaces++;
        }
    }
    (*outTs)->meshComplete();

    GEOMARK_FREE(vertexMap2);

    return 0;
}

/*
  Gegestueck zu filterOutQuads: Die Aenderungen (in den Eckpunktkoordinaten)
  im quad-gefilterten Mesh inTs werden in das Originalmesh outTs uebertragen.
*/
int WmSceneGraphAccess::propagateChanges(TriangleSet *inTs, TriangleSet *outTs,
                                         int *vertexMap)
{
    int i;
    int nv;

    nv = inTs->vertices();

    for(i = 0; i < nv; i++)
    {
        outTs->setVertex(vertexMap[i], inTs->getVertex(i));
    }

    /* maybe perform following cleanup outside function */
    GEOMARK_DELETE(inTs);
    GEOMARK_FREE(vertexMap);

    return 0;
}

/*
  Aufruf dieser Funktion erfolgt bei Traversierung des Szenengraphen
  mittels OpenSG-Funktion traverse().
  Enthaelt ein Knoten verwertbare Geometrieinformation so tragen wir
  Zeiger auf seine Geometrie (OpenSG-Strukturen) im array gla_meshInfo_
  ein.
  Nebenbei bestimmen wir für die Geometrie auch noch die World-Space-
  Transformation (evtl. existiert eine OpenSG-Funktion um diese
  Information zu erhalten, der Autor hat keine in der OpenSG-API
  entdeckt).
*/
Action::ResultE enter(NodePtr &node)
{
    int             i, j, h;
    Pnt3f           v;
    int             numFaces, numFaceVertices, vId, size;

    MeshInfo        meshInfo;
    TinyMatrix      transf;
    FaceIterator    fit;
    int             numQuads;
    NamePtr         namePtr;
    char            name[255];

    namePtr = NamePtr::dcast(node->findAttachment(Name::getClassType().getGroupId()));
    if(namePtr == osg::NullFC)
        strcpy(name, "");
    else
    {
        strcpy(name, namePtr->getFieldPtr()->getValue().c_str());
    }

    SINFO << "Node name = '" << name << "'" << endl << endLog;

    GeometryPtr geo = GeometryPtr::dcast(node->getCore());

    if(geo != NullFC)
    {
        GeoPLengthsUI32Ptr  pLength = GeoPLengthsUI32Ptr::dcast(geo->getLengths());
        GeoPTypesUI8Ptr     pTypes = GeoPTypesUI8Ptr::dcast(geo->getTypes());

        /* pLength and pTypes should not be NullFC, however VRML Importer/Exporter
		  code is instable by now, so this can happen */
        if((pLength != NullFC) && (pTypes != NullFC))
        {
            GeoPLengthsUI32::StoredFieldType * pLengthField = pLength->getFieldPtr();
            GeoPTypesUI8::StoredFieldType * pTypeField = pTypes->getFieldPtr();

            size = pLengthField->size();

            for(h = 0; h < size; h++)
            {
                if(((*pTypeField)[h] == GL_TRIANGLES) ||
                   ((*pTypeField)[h] == GL_QUADS))
                {
                    /* may quads appear in GL_TRIANGLES ? */
                    /* check if all triangles have three vertices */
                    numQuads = 0;
                    fit = geo->beginFaces();
                    while(fit != geo->endFaces())
                    {
                        numFaceVertices = fit.getLength();
                        if(numFaceVertices == 4)
                            numQuads++;
                        if(numFaceVertices > 4)
                        {
                            SWARNING <<
                                "More than 4 vertices in face!" <<
                                endl <<
                                endLog;
                            return Action::Continue;

                            // exit(1);
                        }

                        ++fit;
                    }

                    if(numQuads > 0)
                    {
                        SWARNING << "Quad encountered" << endl << endLog;
                    }

                    if(gl_sga->nodeDepth_ > 0)
                    {
                        for(i = 0; i < gl_sga->nodeDepth_; i++)
                        {
                            meshInfo.transf = meshInfo.transf * gl_sga->transf_[i];
                        }
                    }
                    else
                        meshInfo.transf.identity();

                    /* access to vertices */
                    GeoPositions3fPtr   pPos = GeoPositions3fPtr::dcast(geo->getPositions());
                    GeoPositions3f::StoredFieldType * pPosField = pPos->getFieldPtr();

                    /* access to faces */
                    numFaces = 0;
                    fit = geo->beginFaces();
                    for(fit = geo->beginFaces(); fit != geo->endFaces(); ++fit)
                    {
                        numFaceVertices = fit.getLength();

                        for(j = 0; j < numFaceVertices; j++)
                        {
                            vId = fit.getPositionIndex(j);
                        }

                        numFaces++;
                    }                       /* for fit */

                    /* set other mesh attributes */
                    meshInfo.numQuads = numQuads;
                    meshInfo.geoPtr = geo;
                    meshInfo.vPtr = pPosField;
                    meshInfo.triangularFaces = (numQuads == 0);
                    meshInfo.numVertices = pPosField->size();
                    meshInfo.numFaces = numFaces;

                    gl_sga->meshInfo_.push_back(meshInfo);
                    gl_sga->numGeometryNodes_++;
                }
                else
                {
                    //			SWARNING << "Neither triangle nor quad. Field type = " <<
                    //				        (*pTypeField)[h] << endl << endLog;
                }
            }                               /* for h */
        }                                   /* if pLength!=NullFC */
    }
    else if(node->getCore()->getType().isDerivedFrom(Transform::getClassType()))
    {
        TransformPtr    t = TransformPtr::dcast(node->getCore());
        Matrix          ma;

        ma = t->getMatrix();

        SINFO <<
            "Node type derived from transform, skipping children" <<
            endl <<
            endLog;

        for(i = 0; i < 4; i++)
        {
            for(j = 0; j < 4; j++)
            {
                transf[i][j] = ma[j][i];    /* the matrix is stored as columns/rows ? */
            }
        }

        if(gl_sga->nodeDepth_ > gl_sga->maxNodeDepth_)
        {
            gl_sga->maxNodeDepth_ = gl_sga->nodeDepth_;
            gl_sga->transf_.push_back(transf);
        }
        else
        {
            gl_sga->transf_[gl_sga->nodeDepth_] = transf;
        }

        gl_sga->nodeDepth_++;
    }

    return Action::Continue;
}

/*
  Diese Funktion wird bei Verlassen eines Knotens bei der Traversierung
  aufgerufen. Wir verwenden diese nur, um die aktuelle Tiefe im
  Szenengraphen zu dekrementieren.
*/
Action::ResultE leave(NodePtr &node, Action::ResultE res)
{
    if(node->getCore()->getType().isDerivedFrom(Transform::getClassType()))
    {
        gl_sga->nodeDepth_--;
    }

    return Action::Continue;
}
#endif
