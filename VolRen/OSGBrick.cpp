#include <OSGConfig.h>

#include <OSGBrick.h>

#include <OSGSlicer.h>
#include <OSGDVRVolume.h>
#include <OSGDVRVolumeTexture.h>


#define FACE_FRONT  (1)
#define FACE_BACK   (2)
#define FACE_RIGHT  (4)
#define FACE_LEFT   (8)
#define FACE_BOTTOM (16)
#define FACE_TOP    (32) 


OSG_BEGIN_NAMESPACE


void BrickSet::buildBricks3D(DVRVolume * volume, Vec3f brickSize, int overlap, Orientation ori)
{
    FDEBUG(("BrickSet::buildBricks3D %d %d %d\n", brickSize[0],
	    brickSize[1], brickSize[2]));

    SINFO << "BrickSet::buildBricks3D (size)" << brickSize[0] << ", " << brickSize[1] << ","
	  << brickSize[2] << std::endl;
    
    m_nOrientation = ori;

    DVRVolumeTexturePtr vT = DVRVOLUME_PARAMETER(volume, DVRVolumeTexture);
    if (vT == NullFC) {
        SWARNING << "BrickSet::buildTextures3D - No Volume" << std::endl;
        return;
    }

    if ((brickSize[0] <= overlap) || (brickSize[1] <= overlap) || (brickSize[2] <= overlap))
    {
        SFATAL << "BrickSet::buildBricks3D - brickSize too small: "
	       << brickSize[0] << brickSize[1] << "," << brickSize[2] << std::endl;
	return;
    }
    
    Vec3f vox   = vT->getSliceThickness();
    Vec3f res   = vT->getResolution();
    int   resX  = (int) res[0];
    int   resY  = (int) res[1];
    int   resZ  = (int) res[2];

    int nNumBricksX = int((resX - overlap) / (brickSize[0] - overlap));
    int nNumBricksY = int((resY - overlap) / (brickSize[1] - overlap));
    int nNumBricksZ = int((resZ - overlap) / (brickSize[2] - overlap));

    SINFO << "BrickSet::buildBricks3D (num)" << nNumBricksX << ", " << nNumBricksY << ","
	  << nNumBricksZ << std::endl;
    
    m_nNumBricks = nNumBricksX * nNumBricksY * nNumBricksZ; 
    m_pBricks = new Brick[m_nNumBricks];

    int nBrickSizeX = int(brickSize[0]);
    int nBrickSizeY = int(brickSize[1]);
    int nBrickSizeZ = int(brickSize[2]);

    int nXMin = 0;
    int nYMin = 0;
    int nZMin = 0;

    float xmin = (overlap - 1 + 0.5 - resX/2) * vox[0];
    float ymin = (overlap - 1 + 0.5 - resY/2) * vox[1];
    float zmin = (overlap - 1 + 0.5 - resZ/2) * vox[2];

    float xmax, ymax, zmax;
    
    int index = 0;
	
    for(int z = 0; z < nNumBricksZ; z++) {
		
        nYMin = 0;
	ymin  = (overlap - 1 + 0.5 - resY/2) * vox[1];

	for(int y = 0; y < nNumBricksY; y++) {
			
	    nXMin = 0;
	    xmin  = (overlap - 1 + 0.5 - resX/2) * vox[0];

	    for(int x = 0; x < nNumBricksX; x++) {

	        xmax = xmin + (nBrickSizeX - (2 * (overlap - 1) + 1)) * vox[0]; 
		ymax = ymin + (nBrickSizeY - (2 * (overlap - 1) + 1)) * vox[1];
		zmax = zmin + (nBrickSizeZ - (2 * (overlap - 1) + 1)) * vox[2];
		  
		m_pBricks[index].init(nXMin, nYMin, nZMin,
				      nBrickSizeX, nBrickSizeY, nBrickSizeZ,
				      xmin, ymin, zmin,
				      xmax, ymax, zmax,
				      overlap, ori);
		
		index++;
		nXMin += nBrickSizeX - (2 * overlap - 1);
		xmin = xmax; 
	    }
	    nYMin += nBrickSizeY - (2 * overlap - 1);
	    ymin = ymax;
	}
	
	nZMin += nBrickSizeZ - (2 * overlap - 1);
	zmin = zmax;
    }
}


void BrickSet::buildBricks2D(DVRVolume * volume, Orientation ori)
{
    m_nOrientation = ori;
    
    DVRVolumeTexturePtr vT = DVRVOLUME_PARAMETER(volume, DVRVolumeTexture);
    if (vT == NullFC) return;
    
    Vec3f vox          = vT->getSliceThickness();
    Vec3f res          = vT->getResolution();
    int   resX, resY, resZ;
    float xmin, ymin, zmin;	          
    float xmax, ymax, zmax;          
    float xinc, yinc, zinc;
    
    switch (ori) {
        case XY: {
	    // XY stack of textures
	    resX  = (int) res[0];
	    resY  = (int) res[1];
	    resZ  = 1;

	    xinc = 0;
	    yinc = 0;
	    zinc = 1;

	    xmin = (- (res[0] - 1)/2) * vox[0];  // "geometrical" size of the brick
	    ymin = (- (res[1] - 1)/2) * vox[1];
	    zmin = (- (res[2]    )/2) * vox[2];
    
	    xmax = - xmin;
	    ymax = - ymin;
	    zmax = zmin + zinc * vox[2];

	    m_nNumBricks = (int) res[2];
	    m_pBricks = new Brick[m_nNumBricks];
	    break;
	}
        case XZ: {
	    // XZ stack of textures
	    resX  = (int) res[0];
	    resY  = 1;
	    resZ  = (int) res[2];

	    xinc = 0;
	    yinc = 1;
	    zinc = 0;

	    xmin = (- (res[0] - 1)/2) * vox[0];  // "geometrical" size of the brick
	    ymin = (- (res[1]    )/2) * vox[1];
	    zmin = (- (res[2] - 1)/2) * vox[2];
    
	    xmax = - xmin;
	    ymax = ymin + yinc * vox[1];
	    zmax = - zmin;
	    
	    m_nNumBricks = (int) res[1];
	    m_pBricks = new Brick[m_nNumBricks];
	    break;
	}
        case YZ: {
	    // YZ stack of textures
	    resX  = 1;
	    resY  = (int) res[1];
	    resZ  = (int) res[2];

	    xinc = 1;
	    yinc = 0;
	    zinc = 0;

	    xmin = (- (res[0]    )/2) * vox[0];  // "geometrical" size of the brick
	    ymin = (- (res[1] - 1)/2) * vox[1];
	    zmin = (- (res[2] - 1)/2) * vox[2];
    
	    xmax = xmin + xinc * vox[0];
	    ymax = - ymin;
	    zmax = - zmin;

	    m_nNumBricks = (int) res[0];
	    m_pBricks = new Brick[m_nNumBricks];
	    break;
	}
        case UNDEF: {
	    SFATAL << "BrickSet::buildBricks2D called with unspecified orientation" << std::endl;
	    break;
	}
    }

    for (int index = 0; index < m_nNumBricks; index++) {

        m_pBricks[index].init( resX == 1 ? index : 0,
			       resY == 1 ? index : 0,
			       resZ == 1 ? index : 0,
			       resX, resY, resZ,
			       xmin, ymin, zmin,
			       xmax, ymax, zmax, 1, ori);
	
	xmin += xinc * vox[0];
	xmax += xinc * vox[0];

    	ymin += yinc * vox[1];
	ymax += yinc * vox[1];

    	zmin += zinc * vox[2];
	zmax += zinc * vox[2];
    }
}


//! Create texture chunk for image
TextureChunkPtr BrickSet::makeTexture(UInt32 internalFormat,
				      UInt32 externalFormat,
				      ImageP img) {

    TextureChunkPtr chunk        = TextureChunk::create();

    beginEditCP( chunk );
    chunk->setImage( img );
    chunk->setInternalFormat( internalFormat );
    chunk->setExternalFormat( externalFormat );
    FDEBUG(("TextureManager - makeTexture: intern %d - extern %d\n",
	    internalFormat, externalFormat));

    chunk->setWrapS(GL_CLAMP);
    chunk->setWrapT(GL_CLAMP);
    chunk->setWrapR(GL_CLAMP);
	    
    chunk->setMinFilter(GL_LINEAR);
    chunk->setMagFilter(GL_LINEAR);
    
    chunk->setEnvMode(GL_REPLACE);
    
    endEditCP(chunk);

    return chunk;
}


//! Build textures for bricks
void BrickSet::buildBrickTextures(ChunkMaterialPtr            & material,
				  TextureSet                  & textures,
				  TextureManager::TextureMode   mode)
{
    // overall number of textures
    UInt32 numTextures = textures.size();

    if (mode == TextureManager::TM_2D_Multi)
        numTextures *= 2;
	
    
    beginEditCP(material);

    // build all texture chunks
    for (UInt32 index = 0; index < m_nNumBricks; index++)
    {
        int & resX = m_pBricks[index].voxSize[0];
	int & resY = m_pBricks[index].voxSize[1];
	int & resZ = m_pBricks[index].voxSize[2];

	int & nXMin = m_pBricks[index].minVox[0];
	int & nYMin = m_pBricks[index].minVox[1];
	int & nZMin = m_pBricks[index].minVox[2];
	
	int texCounter = 0;

        m_pBricks[index].texture     = new TextureChunkPtr[numTextures];
	m_pBricks[index].texStage    = new UInt32[numTextures];
	m_pBricks[index].numTextures = numTextures;

	// create texture chunks
	for (UInt32 tch = 0; tch < textures.size(); tch++) { 
	    ImageP          img          = new Image();

	    switch (mode)
	    {
	      case TextureManager::TM_2D_Multi: // 2D multi textures
	      {
	        // first texture
	        textures[tch]->image->slice(resX == 2 ? nXMin : -1,
					    resY == 2 ? nYMin : -1,
					    resZ == 2 ? nZMin : -1,
					    img);
		if (index == 0)
		{
		    m_pBricks[index].texture[texCounter] = makeTexture(textures[tch]->internalFormat,
								       textures[tch]->externalFormat,
								       img);
		}
		else
		{
		    // each but first brick can reuse texture chunks of previous brick
		    m_pBricks[index].texture[texCounter] = m_pBricks[index-1].texture[texCounter+1];
		}
		
		m_pBricks[index].texStage[texCounter] = textures[tch]->textureStage0;
		material->addChunk(m_pBricks[index].texture[texCounter]);
		texCounter++;

		// second texture
		img = new Image();
	        textures[tch]->image->slice(resX == 2 ? nXMin + 1 : -1,
					    resY == 2 ? nYMin + 1 : -1,
					    resZ == 2 ? nZMin + 1 : -1,
					    img);
		m_pBricks[index].texture[texCounter] = makeTexture(textures[tch]->internalFormat,
								   textures[tch]->externalFormat,
								   img);
		m_pBricks[index].texStage[texCounter] = textures[tch]->textureStage1;
		material->addChunk(m_pBricks[index].texture[texCounter]);
		texCounter++;

		break;
	      }
	    
	      case TextureManager::TM_2D: // 2D ordinary textures
	      {
	        textures[tch]->image->slice(resX == 1 ? nXMin : -1,
					    resY == 1 ? nYMin : -1,
					    resZ == 1 ? nZMin : -1,
					    img);
		m_pBricks[index].texture[texCounter] = makeTexture(textures[tch]->internalFormat,
								   textures[tch]->externalFormat,
								   img);
		m_pBricks[index].texStage[texCounter] = textures[tch]->textureStage0;
		material->addChunk(m_pBricks[index].texture[texCounter]);
		texCounter++;

		break;
	      }
	    
	      case TextureManager::TM_3D: // 3D ordinary textures
	      default:
	      {
	        textures[tch]->image->subImage(nXMin, nYMin, nZMin,
					       resX, resY, resZ,
					       img);
		m_pBricks[index].texture[texCounter] = makeTexture(textures[tch]->internalFormat,
								   textures[tch]->externalFormat,
								   img);
		m_pBricks[index].texStage[texCounter] = textures[texCounter]->textureStage0;
		material->addChunk(m_pBricks[index].texture[texCounter]);
		texCounter++;

		break;
	      }
	    }

	} // for all registered textures

    } // for all bricks

    endEditCP(material);
}


//! Reload brick textures
void BrickSet::reloadBrickTextures(DrawActionBase * action, UInt32 texStage)
{
    for (UInt32 i = 0; i < m_nNumBricks; i++)
    {
        for (UInt32 j = 0; j < m_pBricks[i].numTextures; j++)
	{
	    if (m_pBricks[i].texStage[j] == texStage)
	    {
	        action->getWindow()->refreshGLObject(m_pBricks[i].texture[j]->getGLId());
	    }
	}
    }
}


//! Clear brick textures
void BrickSet::clearBrickTextures(ChunkMaterialPtr & material)
{
    for (UInt32 i = 0; i < m_nNumBricks; i++)
    {
        for (UInt32 j = 0; j < m_pBricks[i].numTextures; j++)
	{
	    material->subChunk(m_pBricks[i].texture[j]);
	}
    }
}


//! Return sorted brick list
Brick * BrickSet::sortBricks3D( const Matrix & modelMat, const Vec3f & eyePoint )
{

    Brick *BrickList = m_pBricks;
    int nBrick;
    float distance;
    Brick *previous, * current, * incoming;
	
    Vec3f transformedCenter;
    m_pBricks[0].prev = NULL;
    m_pBricks[0].next = NULL;
    modelMat.multMatrixPnt(m_pBricks[0].center,transformedCenter);
    m_pBricks[0].distance = (eyePoint - transformedCenter).length();
	
    for(nBrick = 1; nBrick < m_nNumBricks; nBrick++) {
        incoming = &(m_pBricks[nBrick]);
	modelMat.multMatrixPnt(incoming->center,transformedCenter);
	distance = (eyePoint - transformedCenter).length();
	incoming->distance  = distance;
	incoming->next = NULL;
	incoming->prev = NULL;
	current = BrickList;
	previous = NULL;
	while((current != NULL) && (current->distance > distance)) {
	    previous = current;
	    current = current->next;
	}
	// insert 
	if (current == NULL) {
	    // insert at the end of the list;
	    previous->next = incoming;
	    incoming->prev = previous;
	  
	} else {
	    incoming->next = current;
	    incoming->prev = current->prev;
	    current->prev = incoming;
	    if (incoming->prev == NULL) {
	        BrickList = incoming;
	    } else {
	        incoming->prev->next = incoming;
	    }
	}
    }
    
    return BrickList;
}


//! Return sorted brick list
Brick * BrickSet::sortBricks2D( bool backToFront )
{
    Brick * brickList = m_pBricks;
    
    if (brickList) {

        if (backToFront) {

	    for (int i = m_nNumBricks - 1; i >= 0; i--) {
	        m_pBricks[i].prev = &(m_pBricks[i + 1]);
	        m_pBricks[i].next = &(m_pBricks[i - 1]);
	    }
	    
	    m_pBricks[0].next                = NULL;
	    m_pBricks[m_nNumBricks - 1].prev = NULL;

	    brickList = &(m_pBricks[m_nNumBricks - 1]);
	  
	} else {
	    for (int i = 0; i < m_nNumBricks; i++) {
	        m_pBricks[i].prev = &(m_pBricks[i - 1]);
	        m_pBricks[i].next = &(m_pBricks[i + 1]);
	    }
	    
	    m_pBricks[0].prev                = NULL;
	    m_pBricks[m_nNumBricks - 1].next = NULL;

	    brickList = &(m_pBricks[0]);
	}
    }
    
    return brickList;
}


//! Initialize brick size and geometry 
void Brick::init(int minVoxX, int minVoxY, int minVoxZ,
		 int resX,    int resY,    int resZ,
		 float xmin, float ymin, float zmin,
		 float xmax, float ymax, float zmax,
		 int overlap, BrickSet::Orientation ori)
{

    FINFO(("Brick::init - upper left (%d, %d, %d)\n",
	   minVoxX, minVoxY, minVoxZ));
    FINFO(("Brick::init (%d, %d, %d): %f %f %f -> %f %f %f \n",
	    resX, resY, resZ, xmin, ymin, zmin, xmax, ymax, zmax));

    float xrange = xmax - xmin;
    float yrange = ymax - ymin;
    float zrange = zmax - zmin;				

    minVox[0] = minVoxX;
    minVox[1] = minVoxY;
    minVox[2] = minVoxZ;

    voxSize[0] = resX;
    voxSize[1] = resY;
    voxSize[2] = resZ;

    m_lowerLeft  = Vec3f(xmin, ymin, zmin);
    m_upperRight = Vec3f(xmax, ymax, zmax);

    m_ori = ori;
    
    float texCoorMin[3], texCoorMax[3];

    texCoorMin[0] = (2 * float(overlap) - 1) / 2.0 / float(resX);
    texCoorMin[1] = (2 * float(overlap) - 1) / 2.0 / float(resY);
    texCoorMin[2] = (2 * float(overlap) - 1) / 2.0 / float(resZ);

    texCoorMax[0] = 1 - texCoorMin[0];
    texCoorMax[1] = 1 - texCoorMin[1];
    texCoorMax[2] = 1 - texCoorMin[2];

    texScale = Vec3f((texCoorMax[0] - texCoorMin[0]) / xrange,
		     (texCoorMax[1] - texCoorMin[1]) / yrange,
		     (texCoorMax[2] - texCoorMin[2]) / zrange);
    texTranslate = Vec3f(texCoorMin[0] - texScale[0] * xmin,
			 texCoorMin[1] - texScale[1] * ymin,
			 texCoorMin[2] - texScale[2] * zmin);
    
    FINFO (("Brick::init - overlap:      %d \n", overlap));
    FINFO (("Brick::init - texScale:     %f %f %f \n", texScale[0], texScale[1], texScale[2]));
    FINFO (("Brick::init - texTranslate: %f %f %f \n", texTranslate[0], texTranslate[1], texTranslate[2]));

    //!! Initialize edge/vertex structure

    //
    //	                     e3       
    //          v5 o----------------o v4
    //            /|               /|
    //           / |              / |
    //      e10 /  |          e9 /  |e7
    //         /   |   e1       /   |
    //     v3 o----------------o v2 |
    //        |    |           |    |
    //        |    |e6         |    |
    //        |e5  |           |e4  |
    //        |    |    e2     |    |
    //        |    o-----------|----o v7
    //        |   /  v6        |   /
    //        |  /             |  /
    //        | /e11           | /e8
    //        |/               |/
    //   y    o----------------o
    //   | z  v0      e0       v1
    //   |/
    //   +---x
				
    // ------------- Initialize 8 Vertices -------------
    vertex[0] = Vec3f( xmin,  ymin,  zmin); 
    vertex[1] = Vec3f( xmax,  ymin,  zmin);
    vertex[2] = Vec3f( xmax,  ymax,  zmin);
    vertex[3] = Vec3f( xmin,  ymax,  zmin);
				
    vertex[4] = Vec3f( xmax,  ymax,  zmax);
    vertex[5] = Vec3f( xmin,  ymax,  zmax); 
    vertex[6] = Vec3f( xmin,  ymin,  zmax);
    vertex[7] = Vec3f( xmax,  ymin,  zmax);

    center = Vec3f(0.0, 0.0, 0.0);
    for(int j = 0; j < 8; j++) {
        center += vertex[j];
    }
    center /= 8.0;
    radius = (vertex[0] - center).length();
		
    // -------------- Initialize 12 Edges --------------
    //                          e0     e1     e2     e3     e4     e5     e6     e7     e8     e9     e10    e11
    const int eindex[12][2] = {{0,1}, {2,3}, {6,7}, {4,5}, {1,2}, {0,3}, {5,6}, {4,7}, {1,7}, {2,4}, {3,5}, {0,6}};
    //                       edge 0                    edge 1                  edge 2                   edge 3
		
    for(int i = 0; i < 12; i++) {
        edges[i]   = Line(Pnt3f(vertex[eindex[i][0]]),
			  Pnt3f(vertex[eindex[i][1]]));
    }
    
#if 0
    TOUT("Init Brick");
    TOUT("Brick (" << x << ", " << y << ", " << z << ")");
    TOUT(VARDUMP(nXMin) << ", " << VARDUMP(nXMax));
    TOUT(VARDUMP(nYMin) << ", " << VARDUMP(nYMax));
    TOUT(VARDUMP(nZMin) << ", " << VARDUMP(nZMax));
    TOUT(VARDUMP(nDataSize));
#endif
}


//! Activate textures
void Brick::activateTexture(DrawActionBase * action)
{
//    FDEBUG(("Brick::activateTexture\n"));
  
    for (int i = 0; i < numTextures; i++) {
//        FDEBUG(("texture %i - Window %d\n", i, action->getWindow()));
 	FDEBUG(("Brick::activateTexture - Brick: %d - Texture %d - Stage: %d - ID: %d\n",
 		this, i, texStage[i], texture[i]->getGLId()));
// 	FDEBUG(("Brick::activateTexture - Image:\n"));
//	texture[i]->getImage()->dump();
        texture[i]->activate(action, texStage[i]);
    }
//    FDEBUG(("activated\n"));
}


//! Change active texture 
void Brick::changeFromTexture(DrawActionBase * action, Brick * old)
{
    FDEBUG(("Brick::changeFromTexture\n"));

    if (old == NULL)
      activateTexture(action);
    
    for (int i = 0; i < numTextures; i++)
    {
        texture[i]->changeFrom(action, &(*(old->texture[i])), texStage[i]);
    }
}


//! Deactivate textures
void Brick::deactivateTexture(DrawActionBase * action)
{
//    FDEBUG(("Brick::deactivateTexture \n"));
    for (int i = 0; i < numTextures; i++) {
//        FDEBUG(("texture $d - Window %d\n", i, action->getWindow()));
        texture[i]->deactivate(action, texStage[i]);
    }
//    FDEBUG(("deactivated\n"));
}


//! Render 2D/3D Slices
void Brick::renderSlices(DVRVolume * volume, DrawActionBase * action,
			 DVRShaderP shader, TextureManager::TextureMode textureMode)
{
    switch (textureMode)
    {
        case TextureManager::TM_2D:
	{
//	    SWARNING << "Brick::render2DSlices" << std::endl;
	    GLboolean val;
//	    glGetBooleanv(GL_TEXTURE_2D, &val);
//	    SWARNING << "Brick::renderSlices - 2DTextures enabled " << (int) val << std::endl;
	    glGetBooleanv(GL_TEXTURE_3D, &val);
	    if (val != 0) {
	        glDisable(GL_TEXTURE_3D);
		SWARNING << "Brick::renderSlices - 3DTextures enabled -> DISABLING " << (int) val << std::endl;
	    }
	    render2DSlices(volume, action, shader);
	    break;
	}
        case TextureManager::TM_2D_Multi:
	{
	    SINFO << "Brick::renderSlices - 2D MTex Slabs - ori("
		  << m_ori << ")" << std::endl;

	    Vec3f sliceDir;
	    int   dir = Slicer::getAASlicingDirection(action, &sliceDir);
	    render2DMultiSlices(volume, action, shader,
				m_ori, Slicer::isBackToFront(dir));
	    break;
	}
        default:
        case TextureManager::TM_3D:
	{
//	    SWARNING << "Brick::render3DSlices" << std::endl;
	    Vec3f sliceDir;
	    (void) Slicer::getSlicingDirection(action, &sliceDir);
	    render3DSlices( volume, action, shader, sliceDir);
	    break;
	}
    }
}


//! Render brick bounding box
void Brick::renderBrick()
{
    FDEBUG(("Brick::renderBrick"));
  
    const int eindex[12][2] = {{0,1}, {2,3}, {6,7}, {4,5}, {1,2}, {0,3}, {5,6}, {4,7}, {1,7}, {2,4}, {3,5}, {0,6}};
    
    glPushAttrib( GL_ENABLE_BIT | GL_CURRENT_BIT );
    glDisable( GL_TEXTURE_3D );
    glDisable( GL_TEXTURE_2D );
    glColor4f( 1.0, 0.0, 0.0, 1.0 );
    
    glBegin(GL_LINES);
    for(int i = 0; i < 12; i++) {
        glVertex3fv( vertex[eindex[i][0]].getValues() );
	glVertex3fv( vertex[eindex[i][1]].getValues() );
    }
    glEnd();

    glPopAttrib();
}


//! Render 3D Slices
void Brick::render3DSlices(DVRVolume      * volume,
			   DrawActionBase * action,
			   DVRShaderP       shader,
			   Vec3f            sliceDir)
{
    int i, j;

    DVRVolumeTexturePtr tex = DVRVOLUME_PARAMETER(volume, DVRVolumeTexture);
    if (tex == NullFC) return;
    
    Vec3f res            = tex->getResolution();
    Vec3f vox            = tex->getSliceThickness();
    float fSampleRate    = volume->getSampling();
    float fSliceDistance = vox.length() / 2.0 / fSampleRate;
    float fTolerance     = fSliceDistance / 100.0;
  
    Vec3f diag(res[0]*vox[0], res[1]*vox[1], res[2]*vox[2]);
    float fRadius = 0.5 * diag.length();
  
    const int ecodes[12] = {
        //     edge 0                    edge 1                  edge 2                   edge 3
        (FACE_FRONT+FACE_BOTTOM), (FACE_FRONT+FACE_TOP),  (FACE_BACK+FACE_BOTTOM), (FACE_BACK+FACE_TOP), 
	//     edge 4                    edge 5                  edge 6                   edge 7 
	(FACE_FRONT+FACE_RIGHT ), (FACE_FRONT+FACE_LEFT), (FACE_BACK+FACE_LEFT),   (FACE_BACK+FACE_RIGHT),
	//     edge 8                    edge 9                  edge 10                  edge 11
	(FACE_RIGHT+FACE_BOTTOM), (FACE_RIGHT+FACE_TOP),  (FACE_TOP+ FACE_LEFT),   (FACE_BOTTOM+FACE_LEFT)};
  
    // get the slicing normal
    Vec3f vecViewDir = sliceDir;
    Plane planeSlice(vecViewDir, -fRadius);
    int numSlices = 1 + (int) (2.0 * fRadius/ fSliceDistance); 
  
    Vec3f   vecIntersections[12];
    int     nEdgeCodes[12];
    bool    bIntersectionUsed[12];
    Vec3f   vecIntersectionsSorted[12];
    Vec3f   vecIntersection;
    int     nSlice, numIntersections, nEdgeCode, numUsedIntersections;
    Vec3f   v, texscale, textrans; 

    texscale = texScale;
    textrans = texTranslate;

    for(nSlice = 0; nSlice < numSlices; nSlice++) {

        // FDEBUG(("Slice $d\n", nSlice));
      
        // calculate intersection with each edge
        numIntersections = 0;
	for(i = 0; i < 12; i++) {
	    if(planeSlice.intersect(edges[i],vecIntersection)) {

	      // SINFO << "edge " << edges[i].getPosition() << " dir " << edges[i].getDirection() << std::endl;
	      // SINFO << "planeSlice " << planeSlice.getNormal() << " " << planeSlice.getDistanceFromOrigin() << std::endl;
	      // SINFO << "vecIntersection " << vecIntersection << std::endl;
	      
	      if ((vecIntersection-center).length() <= radius) {
		  vecIntersections[numIntersections]  = vecIntersection;
		  nEdgeCodes[numIntersections]        = ecodes[i];
		  bIntersectionUsed[numIntersections] = false;
		  numIntersections++;
	      }
	    }
	}
	    
		  
	// eleminate double vertices
	// --------------------------------------------
	if (numIntersections > 2) {
	    for(i = 0; i < numIntersections-1; i++) {
	        for(j = i+1; j < numIntersections; j++) {
		    if ((vecIntersections[i]-vecIntersections[j]).length() < fTolerance) {
		        // merge the two intersections
		        nEdgeCodes[i] |= nEdgeCodes[j];
			// remove j
			if (j < numIntersections-1) {
			    vecIntersections[j] = vecIntersections[numIntersections-1];
			    nEdgeCodes[j] = nEdgeCodes[numIntersections-1];
			    j--; // vertex j has changed, so examine the vertex j again
			}
			numIntersections--;
		    }
		}
	    }
	}
		  
	assert(numIntersections <= 6);
	
	if (numIntersections > 2) {
	  
	    //find correct sequence;
	    vecIntersectionsSorted[0] = vecIntersections[0];
	    bIntersectionUsed[0]      = true;
	    numUsedIntersections      = 1;
			  
	    nEdgeCode = nEdgeCodes[0];
	    while (numUsedIntersections != numIntersections) {
	        for(i = 1; i < numIntersections; i++) {
		    if ((bIntersectionUsed[i] == false) && (nEdgeCodes[i] & nEdgeCode)) {
		        vecIntersectionsSorted[numUsedIntersections] = vecIntersections[i];
			nEdgeCode = nEdgeCodes[i];
			bIntersectionUsed[i] = true;
			numUsedIntersections++;
		    }
		}
	    }
	    
	    if (shader->hasRenderCallback()) {
	        // copy all data into a single field and call rendercallback
	        Real32 data[12 * 6]; // numIntersections is bounded by 12
		UInt32 index = 0;
		for(i = 0; i < numIntersections; i++) {
		    v = vecIntersectionsSorted[i];

		    data[index++] = v[0];
		    data[index++] = v[1];
		    data[index++] = v[2];

		    data[index++] = v[0] * texscale[0] + textrans[0];
		    data[index++] = v[1] * texscale[1] + textrans[1];
		    data[index++] = v[2] * texscale[2] + textrans[2];
		}

		shader->renderSlice (volume, action, data, numIntersections, 6);
	    }
	    else if (volume->getDoTextures()) {
			  
	        glBegin(GL_TRIANGLE_FAN);
		
		for(i = 0; i < numIntersections; i++) {
		    v = vecIntersectionsSorted[i];
		    //glTexCoord3f((v[0] - xmin)/xrange, (v[1] - ymin)/yrange, (v[2] - zmin)/zrange );
		    glTexCoord3f((v[0] * texscale[0] + textrans[0]), 
				 (v[1] * texscale[1] + textrans[1]) , 
				 (v[2] * texscale[2] + textrans[2]));
		    glVertex3fv(v.getValues());

		    FDEBUG(("Brick::render3DSlices V: %f %f %f T: %f %f %f\n",
			    v[0], v[1], v[2],
			    (v[0] * texscale[0] + textrans[0]), 
			    (v[1] * texscale[1] + textrans[1]), 
			    (v[2] * texscale[2] + textrans[2])));
		}
		glEnd();
		
	    } else {
	        glColor4f(1.0, 1.0, 1.0, 1.0);
	        glBegin(GL_LINE_LOOP);
		 
		for(i = 0; i < numIntersections; i++) {
		    v = vecIntersectionsSorted[i];
		    glVertex3fv(v.getValues());
		}
		glEnd();
	    }
	}
		  
	// next plane
	planeSlice.offset(fSliceDistance);
    }
}


void Brick::render2DSlices(DVRVolume * volume, DrawActionBase * action, DVRShaderP shader)
{
    float xmin = vertex[0][0];
    float xmax = vertex[1][0];

    float ymin = vertex[0][1];
    float ymax = vertex[2][1]; 

    float zmin = vertex[0][2];
    float zmax = vertex[4][2]; 

    if ((shader != NullFC) && shader->hasRenderCallback()) {
      // do nothing
    } else if (volume->getDoTextures()) {
        glBegin(GL_TRIANGLE_FAN);
    } else {
        glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
    }


    if (voxSize[2] == 1) {
        //!! XY - slice
      
        if ((shader != NullFC) && shader->hasRenderCallback()) {
            // copy all data into a single field and call rendercallback
	  
	    Real32 data[] = { xmin, ymin, center[2], // first vertex
			      xmin * texScale[0] + texTranslate[0], ymin * texScale[1] + texTranslate[1], 0,
			      xmax, ymin, center[2], // second vertex
			      xmax * texScale[0] + texTranslate[0], ymin * texScale[1] + texTranslate[1], 0,
			      xmax, ymax, center[2], // third vertex
			      xmax * texScale[0] + texTranslate[0], ymax * texScale[1] + texTranslate[1], 0,
			      xmin, ymax, center[2], // fourth vertex
			      xmin * texScale[0] + texTranslate[0], ymax * texScale[1] + texTranslate[1], 0};

	    shader->renderSlice (volume, action, data, 4, 6);
	    
	} else {

  	    glTexCoord2f(xmin * texScale[0] + texTranslate[0],
			 ymin * texScale[1] + texTranslate[1]);
	    glVertex3f(xmin, ymin, center[2]);

 	    FDEBUG(("XY render2dslice (texcoor): %f %f\n",
 		    xmin * texScale[0] + texTranslate[0],
 		    ymin * texScale[1] + texTranslate[1]));
	    
// 	    FDEBUG(("XY render2dslice (coor): %f %f %f\n",
// 		    xmin, ymin, center[2]));

	    glTexCoord2f(xmax * texScale[0] + texTranslate[0],
			 ymin * texScale[1] + texTranslate[1]);
	    glVertex3f(xmax, ymin, center[2]);

	    glTexCoord2f(xmax * texScale[0] + texTranslate[0],
			 ymax * texScale[1] + texTranslate[1]);
	    glVertex3f(xmax, ymax, center[2]);

// 	    FDEBUG(("XY render2dslice (texcoor): %f %f\n",
// 		    xmax * texScale[0] + texTranslate[0],
// 		    ymax * texScale[1] + texTranslate[1]));
	  
// 	    FDEBUG(("XY render2dslice (coor): %f %f %f\n",
// 		    xmax, ymax, center[2]));
	  
	    glTexCoord2f(xmin * texScale[0] + texTranslate[0],
			 ymax * texScale[1] + texTranslate[1]);
	    glVertex3f(xmin, ymax, center[2]);
	}
    }
    else if (voxSize[1] == 1) {
        //!! XZ - slice

        if ((shader != NullFC) && shader->hasRenderCallback()) {
            // copy all data into a single field and call rendercallback
	  
	    Real32 data[] = {xmin, center[1], zmin, // first vertex
			     xmin * texScale[0] + texTranslate[0], zmin * texScale[2] + texTranslate[2], 0,
			     xmax, center[1], zmin, // second vertex
			     xmax * texScale[0] + texTranslate[0], zmin * texScale[2] + texTranslate[2], 0,
			     xmax, center[1], zmax, // third vertex
			     xmax * texScale[0] + texTranslate[0], zmax * texScale[2] + texTranslate[2], 0,
			     xmin, center[1], zmax, // fourth vertex
			     xmin * texScale[0] + texTranslate[0], zmax * texScale[2] + texTranslate[2], 0};
			     
	    shader->renderSlice (volume, action, data, 4, 6);
	    
	} else {
	    glTexCoord2f(xmin * texScale[0] + texTranslate[0],
			 zmin * texScale[2] + texTranslate[2]);
	    glVertex3f(xmin, center[1], zmin);

 	    FDEBUG(("XZ render2dslice (texcoor): %f %f\n",
 		    xmin * texScale[0] + texTranslate[0],
 		    zmin * texScale[2] + texTranslate[2]));
	    
// 	    FDEBUG(("XZ render2dslice (coor): %f %f %f\n",
// 		    xmin, center[1], zmin));
      
	    glTexCoord2f(xmax * texScale[0] + texTranslate[0],
			 zmin * texScale[2] + texTranslate[2]);
	    glVertex3f(xmax, center[1], zmin);

	    glTexCoord2f(xmax * texScale[0] + texTranslate[0],
			 zmax * texScale[2] + texTranslate[2]);
	    glVertex3f(xmax, center[1], zmax);

// 	    FDEBUG(("XZ render2dslice (texcoor): %f %f\n",
// 		    xmax * texScale[0] + texTranslate[0],
// 		    zmax * texScale[2] + texTranslate[2]));

// 	    FDEBUG(("XZ render2dslice (coor): %f %f %f\n",
// 		    xmax, center[1], zmax));
      
	    glTexCoord2f(xmin * texScale[0] + texTranslate[0],
			 zmax * texScale[2] + texTranslate[2]);
	    glVertex3f(xmin, center[1], zmax);
	}
    }
    else {
        //!! YZ - slice
        if ((shader != NullFC) && shader->hasRenderCallback()) {
	    // copy all data into a single field and call rendercallback
	  
	    Real32 data[] = {center[0], ymin, zmin,
			     ymin * texScale[1] + texTranslate[1], zmin * texScale[2] + texTranslate[2], 0,
			     center[0], ymax, zmin,
			     ymax * texScale[1] + texTranslate[1], zmin * texScale[2] + texTranslate[2], 0,
			     center[0], ymax, zmax,
			     ymax * texScale[1] + texTranslate[1], zmax * texScale[2] + texTranslate[2], 0,
			     center[0], ymin, zmax,
			     ymin * texScale[1] + texTranslate[1], zmax * texScale[2] + texTranslate[2], 0};
			     
	    shader->renderSlice (volume, action, data, 4, 6);
	    
	} else {

	    glTexCoord2f(ymin * texScale[1] + texTranslate[1],
			 zmin * texScale[2] + texTranslate[2]);
	    glVertex3f(center[0], ymin, zmin);

 	    FDEBUG(("YZ render2dslice (texcoor): %f %f\n",
 		    ymin * texScale[1] + texTranslate[1],
 		    zmin * texScale[2] + texTranslate[2]));

// 	    FDEBUG(("YZ render2dslice (coor): %f %f %f\n",
// 		    center[0], ymin, zmin));

	    glTexCoord2f(ymax * texScale[1] + texTranslate[1],
			 zmin * texScale[2] + texTranslate[2]);
	    glVertex3f(center[0], ymax, zmin);

	    glTexCoord2f(ymax * texScale[1] + texTranslate[1],
			 zmax * texScale[2] + texTranslate[2]);
	    glVertex3f(center[0], ymax, zmax);
	    
// 	    FDEBUG(("YZ render2dslice (texcoor): %f %f\n",
// 		    ymax * texScale[1] + texTranslate[1],
// 		    zmax * texScale[2] + texTranslate[2]));
	    
// 	    FDEBUG(("YZ render2dslice (coor): %f %f %f\n",
// 		    center[0], ymax, zmax));

	    glTexCoord2f(ymin * texScale[1] + texTranslate[1],
			 zmax * texScale[2] + texTranslate[2]);
	    glVertex3f(center[0], ymin, zmax);
	}
    }


    if ((shader != NullFC) && shader->hasRenderCallback()) {
      // do nothing
    } else {
        glEnd();
    }
}


//! Render Slices 2D with Multi-textures
void Brick::render2DMultiSlices(DVRVolume             * volume,
				DrawActionBase        * action,
				DVRShaderP              shader,
				BrickSet::Orientation   ori,
				bool                    btf)
{
    if (ori > BrickSet::YZ)
    {
        SFATAL << "Brick::render2DMultiSlices - error in orientation" << std::endl;
	return;
    }

    DVRVolumeTexturePtr tex = DVRVOLUME_PARAMETER(volume, DVRVolumeTexture);
    if (tex == NullFC) return;

    Vec3f res            = tex->getResolution();
    Vec3f vox            = tex->getSliceThickness();
    float fSampleRate    = volume->getSampling();

    int dim = 2 - ori;                     // dimension perpenticular to slice planes

    float numSlices = res[dim] * fSampleRate;
    float anchor    = (-res[dim] / 2.0 + 0.5) * vox[dim];  // ancor for the slices
    float dist      = vox[dim] / fSampleRate;

    if (btf)
    {
      SINFO << "Brick::render2DMultiSlices - BTF" << std::endl;
      anchor = -anchor;
      dist   = -dist;
    }
    
    float xmin = m_lowerLeft[0];
    float ymin = m_lowerLeft[1];
    float zmin = m_lowerLeft[2];

    float xmax = m_upperRight[0];
    float ymax = m_upperRight[1];
    float zmax = m_upperRight[2];


    FDEBUG(("Brick::render2DMultiSlices - ori:       %d", ori));
    FDEBUG(("Brick::render2DMultiSlices - numSlices: %d", numSlices));

    for (UInt32 nSlice = 0; nSlice < numSlices; nSlice++)
    {
      // FINFO(("Slice %d: %f\n", nSlice, anchor));

      if ((anchor >= m_lowerLeft[dim]) && (anchor < m_upperRight[dim]))
      {
	FDEBUG(("Slice %d: %f\n", nSlice, anchor));

	if ((shader != NullFC) && shader->hasRenderCallback()) {
	  // do nothing
	} else if (volume->getDoTextures()) {
	  glBegin(GL_TRIANGLE_FAN);
	} else {
	  glColor4f(1.0, 1.0, 1.0, 1.0);
	  glBegin(GL_LINE_LOOP);
	}
	
	    
	switch (ori)
	{
	  case BrickSet::XY:
	  {
	    if ((shader != NullFC) && shader->hasRenderCallback())
	    {
	      // copy all data into a single field and call rendercallback
	      Real32 data[] = {xmin, ymin, anchor, // first vertex
			       xmin   * texScale[0] + texTranslate[0],
			       ymin   * texScale[1] + texTranslate[1],
			       anchor - m_lowerLeft[dim],
			       xmax, ymin, anchor, // second vertex
			       xmax   * texScale[0] + texTranslate[0],
			       ymin   * texScale[1] + texTranslate[1],
			       anchor - m_lowerLeft[dim],
			       xmax, ymax, anchor, // third vertex
			       xmax   * texScale[0] + texTranslate[0],
			       ymax   * texScale[1] + texTranslate[1],
			       anchor - m_lowerLeft[dim],
			       xmin, ymax, anchor, // fourth vertex
			       xmin   * texScale[0] + texTranslate[0],
			       ymax   * texScale[1] + texTranslate[1],
			       anchor - m_lowerLeft[dim]};

	      shader->renderSlice (volume, action, data, 4, 6);
	    }
	    else
	    {
	      glTexCoord2f(xmin * texScale[0] + texTranslate[0],
			   ymin * texScale[1] + texTranslate[1]);
	      glVertex3f(xmin, ymin, anchor);

// 	      FINFO(("XY render2dmultislice (texcoor): %f %f %f\n",
// 		     xmin * texScale[0] + texTranslate[0],
// 		     ymin * texScale[1] + texTranslate[1],
// 		     anchor - m_lowerLeft[dim]));
	    
// 	      FINFO(("XY render2dMulitSlice (coor): %f %f %f\n",
// 		     xmin, ymin, anchor));

	      glTexCoord2f(xmax * texScale[0] + texTranslate[0],
			   ymin * texScale[1] + texTranslate[1]);
	      glVertex3f(xmax, ymin, anchor);

	      glTexCoord2f(xmax * texScale[0] + texTranslate[0],
			   ymax * texScale[1] + texTranslate[1]);
	      glVertex3f(xmax, ymax, anchor);
	    
// 	      FINFO(("XY render2dmultislice (texcoor): %f %f %f\n",
// 		     xmin * texScale[0] + texTranslate[0],
// 		     ymin * texScale[1] + texTranslate[1],
// 		     anchor - m_lowerLeft[dim]));
	  
// 	      FINFO(("XY render2dmultislice (coor): %f %f %f\n",
// 		     xmax, ymax, anchor));
	  
	      glTexCoord2f(xmin * texScale[0] + texTranslate[0],
			   ymax * texScale[1] + texTranslate[1]);
	      glVertex3f(xmin, ymax, anchor);
	    }
	    break;
	  } //!! XY slice

	  case BrickSet::XZ:
	  {
	    if ((shader != NullFC) && shader->hasRenderCallback())
	    {
	      // copy all data into a single field and call rendercallback
	      Real32 data[] = {xmin, anchor, zmin, // first vertex
			       xmin * texScale[0] + texTranslate[0],
			       zmin * texScale[2] + texTranslate[2],
			       anchor - m_lowerLeft[dim],
			       xmax, anchor, zmin, // second vertex
			       xmax * texScale[0] + texTranslate[0],
			       zmin * texScale[2] + texTranslate[2],
			       anchor - m_lowerLeft[dim],
			       xmax, anchor, zmax, // third vertex
			       xmax * texScale[0] + texTranslate[0],
			       zmax * texScale[2] + texTranslate[2],
			       anchor - m_lowerLeft[dim],
			       xmin, anchor, zmax, // fourth vertex
			       xmin * texScale[0] + texTranslate[0],
			       zmax * texScale[2] + texTranslate[2],
			       anchor - m_lowerLeft[dim]};
			     
	      shader->renderSlice (volume, action, data, 4, 6);
	    }
	    else
	    {
	      glTexCoord2f(xmin * texScale[0] + texTranslate[0],
			   zmin * texScale[2] + texTranslate[2]);
	      glVertex3f(xmin, anchor, zmin);

// 	    FDEBUG(("XZ render2dmultislice (texcoor): %f %f\n",
// 		    xmin * texScale[0] + texTranslate[0],
// 		    zmin * texScale[2] + texTranslate[2]));
	    
// 	    FDEBUG(("XZ render2dmultislice (coor): %f %f %f\n",
// 		    xmin, anchor, zmin));
      
	      glTexCoord2f(xmax * texScale[0] + texTranslate[0],
			   zmin * texScale[2] + texTranslate[2]);
	      glVertex3f(xmax, anchor, zmin);

	      glTexCoord2f(xmax * texScale[0] + texTranslate[0],
			   zmax * texScale[2] + texTranslate[2]);
	      glVertex3f(xmax, anchor, zmax);

// 	    FDEBUG(("XZ render2dmultislice (texcoor): %f %f\n",
// 		    xmax * texScale[0] + texTranslate[0],
// 		    zmax * texScale[2] + texTranslate[2]));

// 	    FDEBUG(("XZ render2dmultislice (coor): %f %f %f\n",
// 		    xmax, anchor, zmax));
      
	      glTexCoord2f(xmin * texScale[0] + texTranslate[0],
			   zmax * texScale[2] + texTranslate[2]);
	      glVertex3f(xmin, anchor, zmax);
	    }
	    break;
	  } //!! XZ slice

	  case BrickSet::YZ:
	  {
	    if ((shader != NullFC) && shader->hasRenderCallback())
	    {
	      // copy all data into a single field and call rendercallback
	      Real32 data[] = {anchor, ymin, zmin, // first vertex
			       ymin * texScale[1] + texTranslate[1],
			       zmin * texScale[2] + texTranslate[2],
			       anchor - m_lowerLeft[dim],
			       anchor, ymax, zmin, // second vertex
			       ymax * texScale[1] + texTranslate[1],
			       zmin * texScale[2] + texTranslate[2],
			       anchor - m_lowerLeft[dim],
			       anchor, ymax, zmax, // third vertex
			       ymax * texScale[1] + texTranslate[1],
			       zmax * texScale[2] + texTranslate[2],
			       anchor - m_lowerLeft[dim],
			       anchor, ymin, zmax, // fourth vertex
			       ymin * texScale[1] + texTranslate[1],
			       zmax * texScale[2] + texTranslate[2],
			       anchor - m_lowerLeft[dim]};
			     
	      shader->renderSlice (volume, action, data, 4, 6);
	    }
	    else
	    {
	      glTexCoord2f(ymin * texScale[1] + texTranslate[1],
			   zmin * texScale[2] + texTranslate[2]);
	      glVertex3f(anchor, ymin, zmin);

// 	    FDEBUG(("YZ render2dmultislice (texcoor): %f %f\n",
// 		    ymin * texScale[1] + texTranslate[1],
// 		    zmin * texScale[2] + texTranslate[2]));

// 	    FDEBUG(("YZ render2dmultislice (coor): %f %f %f\n",
// 		    anchor, ymin, zmin));

	      glTexCoord2f(ymax * texScale[1] + texTranslate[1],
			   zmin * texScale[2] + texTranslate[2]);
	      glVertex3f(anchor, ymax, zmin);
	      
	      glTexCoord2f(ymax * texScale[1] + texTranslate[1],
			   zmax * texScale[2] + texTranslate[2]);
	      glVertex3f(anchor, ymax, zmax);
	    
// 	    FDEBUG(("YZ render2dmultislice (texcoor): %f %f\n",
// 		    ymax * texScale[1] + texTranslate[1],
// 		    zmax * texScale[2] + texTranslate[2]));
	    
// 	    FDEBUG(("YZ render2dmultislice (coor): %f %f %f\n",
// 		    anchor, ymax, zmax));

	      glTexCoord2f(ymin * texScale[1] + texTranslate[1],
			   zmax * texScale[2] + texTranslate[2]);
	      glVertex3f(anchor, ymin, zmax);
	    }
	    break;
	  } //!! YZ - slice

	  case BrickSet::UNDEF: {
	      SFATAL << "Brick::render2DMultiSlices called with unspecified orientation" << std::endl;
	      break;
	  }
	}

	if ((shader != NullFC) && shader->hasRenderCallback()) {
	  // do nothing
	} else {
	  glEnd();
	}
      }

      // next slice
      anchor += dist;
    }
}


OSG_END_NAMESPACE


