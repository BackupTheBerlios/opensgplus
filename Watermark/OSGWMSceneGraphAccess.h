#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)

/*
  Die Klasse WmSceneGraphAccess stellt Zugriff auf den
  OpenSG-Szenengraphen bereit und sorgt fuer die Kovertierung
  von OpenSG-Meshes in den von den Geomark-Methoden
  erwarteten Typ TriangleSet.

  Verarbeitet werden koennen nur Meshes bestehend aus Dreiecksflaechen
  oder Quads.
*/
#ifndef _WMSCENEGRAPHACCESS_H_
#define _WMSCENEGRAPHACCESS_H_

OSG_USING_NAMESPACE

// New method definitions for callbacks in OpenSG 1.2. WF 03.07.2003
Action::ResultE enter(NodePtr &node);
Action::ResultE leave(NodePtr &node, Action::ResultE res);

/* eine Instanz der folgenden Klasse nimmt die Information
  genau eines IndexedFaceSets auf */
class           MeshInfo
{
public:

    int         meshId;
    GeometryPtr geoPtr;
    GeoPositions3f::StoredFieldType * vPtr;
    int         numVertices, numFaces;
    BOOL        triangularFaces;
    TinyMatrix  transf;
    int         numQuads;

    MeshInfo(void)
    {
        meshId = -1;
        geoPtr = NullFC;
        vPtr = NULL;
        numVertices = 0;
        numFaces = 0;
        triangularFaces = FALSE;
        transf.identity();
        numQuads = 0;
    }

    MeshInfo &operator=(const MeshInfo &elem)
    {
        meshId = elem.meshId;
        geoPtr = elem.geoPtr;
        vPtr = elem.vPtr;
        numVertices = elem.numVertices;
        numFaces = elem.numFaces;
        triangularFaces = elem.triangularFaces;
        transf = elem.transf;
    }

    bool operator < (const MeshInfo &elem) const
    {
        return vPtr < elem.vPtr;
    }
    bool operator==(const MeshInfo &elem) const
    {
        return vPtr == elem.vPtr;
    }

    void dump(void)
    {
        SINFO << "meshId      = " << meshId << endl;
        SINFO << "numQuads    = " << numQuads << endl;
        SINFO << "vPtr        = " << vPtr << endl;
        SINFO << "numVertices = " << numVertices << endl;
        SINFO << "numFaces    = " << numFaces << endl;
        if(OSG::osgLogP->checkLevel(OSG::LOG_INFO))
        {
            SINFO << "transf      = " << endl;
            transf.dump();
            cout << endl;
        }
    }
};

class   WmSceneGraphAccess
{
public:

    int                 numGeometryNodes_;                              /* we count only geometry nodes containing GL_TRIANGLES */
    int                 nodeDepth_;                                     /* only entering transformation nodes increments this value */
    int                 maxNodeDepth_;
    int                 numMeshes_;

    vector<MeshInfo>    meshInfo_;
    vector<TinyMatrix>  transf_;

    WmSceneGraphAccess(NodePtr *pScene);
    ~WmSceneGraphAccess(void);

    /* GeometryNode to TriangleSet conversion */
    int convertToTs(TriangleSet *ts, int *numQuadsFound,
                    GEOMARK_BitArray **isQuadVertex, int meshId = -1);  /* -1 means generate single flat mesh */
    int writeBack(TriangleSet *ts, int meshId = -1);

    /* quad filtering */
    int filterOutQuads(TriangleSet *inTs, TriangleSet **outTs,
                       GEOMARK_BitArray *isQuadVertex, int **vertexMap,
                       double *bbDiameter);
    int propagateChanges(TriangleSet *inTs, TriangleSet *outTs, int *vertexMap);
};
#endif
#endif
