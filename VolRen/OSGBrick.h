#ifndef _OSGBRICK_H_
#define _OSGBRICK_H_

#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>

#include <OSGVolRenDef.h>

#include <vector>

#include <OSGVector.h>
#include <OSGChunkMaterial.h>
#include <OSGTextureChunk.h>
#include <OSGImage.h>

#include <OSGDVRShader.h>
#include <OSGTextureManager.h>


OSG_BEGIN_NAMESPACE


// Forward declarations
class Brick;
class DVRVolume;


struct BrickSet {
    // Orientation values must be fix for they are used for indexing
    enum Orientation {
        XY    = 0,
	XZ    = 1,
	YZ    = 2,
	UNDEF = 3   // especially for 3D textures
    };
    Brick            *m_pBricks;
    int               m_nNumBricks;
    Orientation       m_nOrientation;
  
    BrickSet() : m_pBricks(0), m_nNumBricks(0), m_nOrientation(UNDEF) {};
  
    //! Return sorted brick list
    Brick * sortBricks3D( const Matrix & modelMat, const Vec3f & eyePoint );
    Brick * sortBricks2D( bool backToFront );

    //! Build textures for bricks
    void buildBrickTextures(ChunkMaterialPtr & material, TextureSet & textures,
			    TextureManager::TextureMode mode);

    //! Reload brick textures
    void reloadBrickTextures(DrawActionBase * action, UInt32 texStage);

    //! Clear brick textures
    void clearBrickTextures(ChunkMaterialPtr & material);
    
    //! Build 3D bricks
    void buildBricks3D(DVRVolume * volume, Vec3f brickSubdivision, int overlap = 1,
		       Orientation ori = UNDEF);
  
    //! Build 2D bricks
    void buildBricks2D(DVRVolume * volume, Orientation ori);

    //! Create texture chunk for image
    static TextureChunkPtr makeTexture(UInt32 internalFormat, UInt32 externalFormat, ImageP image);
};


class Brick {

private:
    Vec3f                 vertex[8];
    Line                  edges[12];
    TextureChunkPtr     * texture;
    UInt32              * texStage;
    int                   numTextures;
    Vec3f                 center;
    float                 radius;
    Vec3f                 texTranslate;
    Vec3f                 texScale;
    int                   voxSize[3];    // Voxelspace size of the brick
    int                   minVox[3];
    Vec3f                 m_lowerLeft;   // Geometrical size of the brick
    Vec3f                 m_upperRight;
    BrickSet::Orientation m_ori;

    // temporary values
    float                 distance;
    Brick               * prev;
    Brick               * next;

public:
    //! Returns next brick in sorted order
    Brick * getNext() {return next;};
    
    //! Initialize brick size and geometry
    void init(int   minVoxX, int   minVoxY, int   minVoxZ,
	      int   resX,    int   resY,    int   resZ,
	      float xMin,    float yMin,    float zMin,
	      float xMax,    float yMax,    float zMax,
	      int   overlap = 1,
	      BrickSet::Orientation ori = BrickSet::UNDEF);

    //! Activate textures
    void activateTexture  (DrawActionBase * action);
    void changeFromTexture(DrawActionBase * action, Brick * old);
    void deactivateTexture(DrawActionBase * action);

    //! Render slices
    void renderSlices(DVRVolume * volume, DrawActionBase * action,
		      DVRShaderP shader,  TextureManager::TextureMode mode);

    //! Render brick boundaries
    void renderBrick();
    
private:
    //! Render slices
    void render3DSlices(DVRVolume * volume, DrawActionBase * action,
			DVRShaderP  shader, Vec3f sliceDir);
    void render2DSlices(DVRVolume * volume, DrawActionBase * action, DVRShaderP shader);
    void render2DMultiSlices(DVRVolume             * volume,
			     DrawActionBase        * action,
			     DVRShaderP              shader,
			     BrickSet::Orientation   ori,
			     bool                    btf);

    friend class BrickSet;
};


OSG_END_NAMESPACE

#endif
