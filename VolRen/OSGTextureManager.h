#ifndef _OSGTEXTUREMANAGER_H_
#define _OSGTEXTUREMANAGER_H_

#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>

#include <OSGVolRenDef.h>

#include <OSGChunkMaterial.h>
#include <OSGImage.h>

#include <vector>


OSG_BEGIN_NAMESPACE


// forward declarations

class DVRVolume;
class BrickSet;
class Brick;
class DrawActionBase;

/*! \brief *put brief class description here* 
  Auxiliary class for DVRVolume. It is intended for texture
  management and bricking.
 */

struct TextureRecord {
    ImagePtr image;
    UInt32   internalFormat;
    UInt32   externalFormat;
    Int32    textureStage0;
    Int32    textureStage1;
  
    TextureRecord(ImagePtr _img, UInt32 _internal, UInt32 _externalFormat,
		  Int32 _stage0, Int32 _stage1);
    ~TextureRecord();

private:
    //!! We do not want the copy constructor to be called -
    //!! would require deep copy
    TextureRecord(const TextureRecord &source);
};


typedef std::vector<TextureRecord*> TextureSet;


class TextureManager {

public:
    //! Texture mode
    enum TextureMode {
        TM_3D = 0,
        TM_2D,
        TM_2D_Multi,
        NUM_TEXTURE_MODES
    };

    //! Bricking mode
    enum BrickingMode {
        BRICK_SUBDIVIDE_ON_TEXTURE_MEMORY = 0,
	BRICK_SUBDIVIDE_STATIC,
	BRICK_SUBDIVIDE_ON_BRICK_SIZE,
	NUM_BRICKING_MODES
    };
    
    //! Constructor/ destructor
    TextureManager(DVRVolume * volume);
    ~TextureManager();

    //! Register texture
    /*! \brief register an image for use as texture with direct volume rendering
     *         image                   points to the image
     *         internal/externalFormat defines the texture format
     *	       doBricking              specifies, whether the texture is uses as utility
     *	                               texture or as volume texture (volume textures are
     *                                 divided into bricks - utility textures are loaded
     *                                 as they are)
     *	       textureState0
     *         textureState1           (optional) specifies which (multi-)texture stages
     *	 			       the texture shall be bound
     */
    Int32 registerTexture(ImagePtr image,
			  UInt32   internalFormat,
			  UInt32   externalFormat,
			  bool     doBricking    = 1,
			  Int32    textureStage0 = 0,
			  Int32    textureStage1 = -1);
    
    //! Remove texture
    void unregisterTexture(Int32 id);

    //! Reload texture
    void reloadTexture(Int32 id, DrawActionBase * action);

    //! Clear all registered textures
    void clearTextures(ChunkMaterialPtr material = NullFC);
	  
    //! Create texture chunks for registered textures
    void buildTextures(ChunkMaterialPtr material, DVRVolume * volume, TextureMode mode = TM_3D);

    //! Return sorted brick list
    Brick * sortBricks(DrawActionBase *da, Matrix modelMat, Vec3f eyePoint, DVRVolume * volume, TextureMode mode);
    
    //! Output the instance for debug purposes
    void dump(      UInt32     uiIndent = 0, 
		    const BitVector  bvFlags  = 0) const;
	  
private:
    TextureSet   registeredTextures;

    DVRVolume   *parent;

    BrickSet    *m_BrickSets;

    //! Calculate bricking 
    static Vec3f calcBrickSubdivision(int resX, int resY, int resZ, int dataSize, DVRVolume * volume);

    friend struct BrickSet;
};

OSG_END_NAMESPACE

#endif
