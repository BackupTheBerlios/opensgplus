#include <OSGConfig.h>

#include "OSGTextureManager.h"

#include <OSGTextureChunk.h>
#include <OSGDrawActionBase.h>
#include <OSGMatrix.h>
#include <OSGVector.h>
#include <OSGQuaternion.h>
#include <OSGLog.h>

#include "OSGDVRVolumeTexture.h"
#include "OSGDVRVolume.h"
#include "OSGSlicer.h"
#include "OSGBrick.h"


#define FACE_FRONT  (1)
#define FACE_BACK   (2)
#define FACE_RIGHT  (4)
#define FACE_LEFT   (8)
#define FACE_BOTTOM (16)
#define FACE_TOP    (32) 


OSG_BEGIN_NAMESPACE

//!! --- CLASS TextureRecord ------------------------------------------------------

TextureRecord::TextureRecord(ImageP _img, UInt32 _internal, UInt32 _externalFormat,
			     Int32 _stage0, Int32 _stage1, bool _doCopy) :
    internalFormat(_internal),
    externalFormat(_externalFormat),
    textureStage0(_stage0),
    textureStage1(_stage1),
    doCopy(_doCopy)
{
#ifdef MW_OSG_1_1
    if (doCopy) {
        image = new Image(*_img);
//	std::cerr << "copy image: " << int(image) << std::endl;
    }
    else {
        image = _img;
//	std::cerr << "reference image: " << int(image) << std::endl;
    }
#else
    image = _img;
    image->addRef();
#endif
}

TextureRecord::~TextureRecord()
{
#ifdef MW_OSG_1_1  
    if (doCopy) {
//      std::cerr << "deleting image: " << int(image) << std::endl;
      delete image;
    }
#else
    image->subRef();
    image = NULL;
#endif
}


//!! --- CLASS TextureManager ------------------------------------------------------


TextureManager::TextureManager(DVRVolume * volume)
{
    parent        = volume;
    m_BrickSets   = new BrickSet[3];
}

TextureManager::~TextureManager()
{
    clearTextures();
    delete [] m_BrickSets;
}

//! Register texture
Int32 TextureManager::registerTexture(ImageP image,
				      UInt32 internalFormat,
				      UInt32 externalFormat,
				      bool   doBricking,
				      Int32  textureStage0,
				      Int32  textureStage1,
				      bool   doCopy)
{ 
    FDEBUG(("TextureManager::registerTexture stage0: %d - stage1: %d\n",
	    textureStage0, textureStage1));

    //!! TODO:
    //!! make shure the user specifies two texture stages for mt-slab mode
    //!! (e.g. accept only textures with two stages set once two stages have
    //!!       been specified).
    
    // check for unique texture stages
    bool unique = true;
    for (UInt32 i = 0; i < registeredTextures.size(); i++)
    {
        if (textureStage0 == registeredTextures[i]->textureStage0 ||
	    textureStage0 == registeredTextures[i]->textureStage1)
	{
	    unique = false;
	}
	if ((textureStage1 != -1) &&
	    (textureStage1 == registeredTextures[i]->textureStage0 ||
	     textureStage1 == registeredTextures[i]->textureStage1))
	{
	    unique = false;
	}
    }

    if (!unique)
    {
        SWARNING << "TextureManager::registerTexture - "
		 << "ID already in use -> ignoring texture" << std::endl;
	return -1;
    }

    // store record
    TextureRecord * record = new TextureRecord(image,
					       internalFormat,
					       externalFormat,
					       textureStage0,
					       textureStage1,
					       doCopy);
    registeredTextures.push_back(record);
    
    return registeredTextures.size() - 1; 
}


//! Remove texture
void TextureManager::unregisterTexture(Int32 id)
{
    FDEBUG(("TextureManager::unregisterTexture\n"));
    if ((id < 0) || (id > registeredTextures.size()))
    {
        SWARNING << "TextureManager::unregisterTexture - invalid id " << id << std::endl;
	return;
    }

#if 1
    TextureSet::iterator iter = registeredTextures.begin();
    delete registeredTextures[id];
    registeredTextures.erase(iter+id);
#else
    for (TextureSet::iterator iter = registeredTextures.begin();
	 iter != registeredTextures.end();
	 iter++)
    {
        if ((*iter).image == image) {
	    FDEBUG(("TextureManager::untegisterTexture - Texture found - %d \n", iter));
	    delete registeredTextures[iter];
	    registeredTextures.erase(iter);
	}
    }
#endif
}


//! Reload texture
void TextureManager::reloadTexture(Int32 id, DrawActionBase * action)
{
    if ((id < 0) || (id > registeredTextures.size()))
    {
        SWARNING << "TextureManager::reloadTexture - invalid id " << id << std::endl;
	return;
    }

#if 1
    for (int k = 0; k < 3; k++)
    {
        m_BrickSets[k].reloadBrickTextures(action, registeredTextures[id]->textureStage0);
	m_BrickSets[k].reloadBrickTextures(action, registeredTextures[id]->textureStage1);
    }
#else
    
    FDEBUG(("TextureManager::reloadTexture\n"));
    for (int i = 0; i < registeredTextures.size(); i++) {
        if (registeredTextures[i]->image == image) {
	    FDEBUG(("TextureManager::reloadTexture - Texture found\n"));
	    FDEBUG(("texture %d - Window %d\n", i, action->getWindow()));

	    for (int k = 0; k < 3; k++)
	    {
	        m_BrickSets[k].reloadBrickTextures(action, registeredTextures[i]->textureStage0);
		m_BrickSets[k].reloadBrickTextures(action, registeredTextures[i]->textureStage1);
	    }
	}
    }
#endif
}


//! Clear all registered textures
void TextureManager::clearTextures(ChunkMaterialPtr material)
{
    FDEBUG(("TextureManager::clearTexture\n"));

    //!! TODO: destroy created texture chunks - even if material is not known

    if (material != NullFC) {
      for (int k = 0; k < 3; k++) {
	  m_BrickSets[k].clearBrickTextures(material);

	  delete [] m_BrickSets[k].m_pBricks;
	  m_BrickSets[k].m_pBricks    = 0;
	  m_BrickSets[k].m_nNumBricks = 0;
      }
    }
     
    for (int i = 0; i < registeredTextures.size(); i++)
      delete registeredTextures[i];
      
    registeredTextures.clear();
}
	  

//! Create texture chunks for registered textures
void TextureManager::buildTextures(ChunkMaterialPtr   material,
				   DVRVolume        * volume,
				   TextureMode        textureMode)
{
    FDEBUG(("TextureManager::buildTextures\n"));
    DVRVolumeTexturePtr vT = DVRVOLUME_PARAMETER(volume, DVRVolumeTexture);
    if (vT == NullFC) {
        SWARNING << "TextureManager::buildTextures - No Volume" << std::endl;
        return;
    }
    if (registeredTextures.size() < 1) {
        SWARNING << "TextureManager::buildTextures - No registerd textures" << std::endl;
	return;
    }

    
    switch (textureMode)
    {
        case TM_2D_Multi:
	{
	    // Multi-Texture Slabs
	    Vec3f res = vT->getResolution();
	    Vec3f brickSizeX(2, res[1], res[2]);
	    Vec3f brickSizeY(res[0], 2, res[2]);
	    Vec3f brickSizeZ(res[0], res[1], 2);

	    std::cerr << "TextureManager::buildTextures " << res[0] << ", " << res[1] << "," << res[2] << std::endl;
	
	    m_BrickSets[BrickSet::XY].buildBricks3D(volume, brickSizeZ, 1, BrickSet::XY);
	    m_BrickSets[BrickSet::XZ].buildBricks3D(volume, brickSizeY, 1, BrickSet::XZ);
	    m_BrickSets[BrickSet::YZ].buildBricks3D(volume, brickSizeX, 1, BrickSet::YZ);

	    m_BrickSets[BrickSet::XY].buildBrickTextures(material, registeredTextures, textureMode); 
	    m_BrickSets[BrickSet::XZ].buildBrickTextures(material, registeredTextures, textureMode); 
	    m_BrickSets[BrickSet::YZ].buildBrickTextures(material, registeredTextures, textureMode); 

	    break;
	}
        case TM_2D:
	{
	    // Ordinary 2D-Slices
	    m_BrickSets[BrickSet::XY].buildBricks2D(volume, BrickSet::XY);
	    m_BrickSets[BrickSet::XZ].buildBricks2D(volume, BrickSet::XZ);
	    m_BrickSets[BrickSet::YZ].buildBricks2D(volume, BrickSet::YZ);

	    m_BrickSets[BrickSet::XY].buildBrickTextures(material, registeredTextures, textureMode); 
	    m_BrickSets[BrickSet::XZ].buildBrickTextures(material, registeredTextures, textureMode); 
	    m_BrickSets[BrickSet::YZ].buildBrickTextures(material, registeredTextures, textureMode); 

	    break;
	}
        default:
        case TM_3D:
	{
            // Ordinary 3D-Bricks
	    Vec3f res              = vT->getResolution();
	    Vec3f brickSubdivision = calcBrickSubdivision(int(res[0]), int(res[1]), int(res[2]),
							  vT->getImage()->getBpp(), volume);
	    Vec3f brickSize((int)(res[0] / brickSubdivision[0]),
			    (int)(res[1] / brickSubdivision[1]),
			    (int)(res[2] / brickSubdivision[2]));

	    FDEBUG(("TextureManager::buildTextures - BrickSubdivision %d %d %d\n",
		    brickSubdivision[0], brickSubdivision[1], brickSubdivision[2]));

	    m_BrickSets[BrickSet::XY].buildBricks3D(volume, brickSize, volume->getBrickOverlap());
	    
	    m_BrickSets[BrickSet::XY].buildBrickTextures(material, registeredTextures, textureMode);

	    break;
	}
    }
}


//! Output the instance for debug purposes
void TextureManager::dump(      UInt32     /*uiIndent*/, 
			  const BitVector  /*bvFlags*/) const
{
    SLOG << "TextureManager:" << std::endl;

    for (UInt32 i = 0; i < registeredTextures.size(); i++)
    {
      SLOG << "image:          " << registeredTextures[i]->image << std::endl;
      SLOG << "internalFormat: " << registeredTextures[i]->internalFormat << std::endl;
      SLOG << "textureStage0:  " << registeredTextures[i]->textureStage0 << std::endl;
      SLOG << "textureStage1:  " << registeredTextures[i]->textureStage1 << std::endl;
    } 
}


Vec3f TextureManager::calcBrickSubdivision(int resX, int resY, int resZ, int dataSize, DVRVolume * volume)
{
        int mode = volume->getBrickingMode();


        Vec3f subdivision(1,1,1);

        switch (mode) {
        case BRICK_SUBDIVIDE_STATIC:
                {
                        subdivision = volume->getBrickStaticSubdivision();
                        break;
                }

        case BRICK_SUBDIVIDE_ON_TEXTURE_MEMORY:
                {
		        float fMB_available = 1024.0F *1024.0F *(float) volume->getBrickStaticMemoryMB();
                        float fMB_required  = (float) (resX * resY * resZ * dataSize);
                        int nNumBricks      = (int) ceil(fMB_required / fMB_available);
                        int nRealNumBricks   = 1;
                        while(nNumBricks > nRealNumBricks) {

                                if ((resX >= resY) && (resX >= resZ)) {
                                        resX /= 2;
                                        subdivision[0] *= 2;
                                } else if (resY >= resZ) {
                                        resY /= 2;
                                        subdivision[1] *= 2;

                                } else {
                                        resZ /= 2;
                                        subdivision[2] *= 2;
                                }

                                nRealNumBricks = (int)(subdivision[0]*subdivision[1]*subdivision[2]);
                        }
                        break;
                }

        case BRICK_SUBDIVIDE_ON_BRICK_SIZE:
                {
                        Vec3f maxSize = volume->getBrickMaxSize();

                        while(resX > (int) maxSize[0]) {
                                resX /= 2;
                                subdivision[0] *= 2;
                        }
                        while(resY > (int) maxSize[1]) {
                                resY /= 2;
                                subdivision[1] *= 2;
                        }
                        while(resZ > (int) maxSize[2]) {
                                resZ /= 2;
                                subdivision[2] *= 2;
                        }
                        break;
                }

        }
        return subdivision;
}


Brick *
TextureManager::sortBricks(Matrix modelMat, Vec3f eyePoint, DVRVolume * volume, TextureMode mode) {
	
    FDEBUG(("TextureManager::sortBricks\n"));
    if ((mode == TM_2D) || (mode == TM_2D_Multi)) {
        // select the right bickset
        Vec3f               finalSliceDir;
	static Vec3f        volTranslation;
	static Vec3f        volScale;
	static Quaternion   volRotationInv;
	static Quaternion   dummyRot;	// static over all instances

	modelMat.getTransform( volTranslation, volRotationInv, volScale, dummyRot ); 
	volRotationInv.invert();
	volRotationInv.multVec( eyePoint, finalSliceDir );
	
	double x = finalSliceDir[0];
	double y = finalSliceDir[1];
	double z = finalSliceDir[2];
	double X = (x > 0)? x: -x;
	double Y = (y > 0)? y: -y;
	double Z = (z > 0)? z: -z;
	
	BrickSet::Orientation ori = BrickSet::XY;
	bool                  btf = true;
	
	if ((Z > Y) && (Z > X)) { // Z is the largest component
	  ori = BrickSet::XY;
	  if (z > 0) {
	    btf = false;
	  } else {
	    btf = true;
	  }
	} else {
	  if (Y > X) {
	    ori = BrickSet::XZ;
	    if (y > 0) {
	      btf = false;
	    } else {
	      btf = true;
	    }
	  } else {
	    ori = BrickSet::YZ;
	    if (x > 0) {
	      btf = false;
	    } else {
	      btf = true;
	    }
	  }
	}

	// btf or ftb? 
        return m_BrickSets[ori].sortBricks2D( btf );
    } else {
        return m_BrickSets[BrickSet::XY].sortBricks3D( modelMat, eyePoint );
    }
}


OSG_END_NAMESPACE


