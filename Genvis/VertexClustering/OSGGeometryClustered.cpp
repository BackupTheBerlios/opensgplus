/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "OSGGeometryClustered.h"
#include <OSGConfig.h>
#include <OSGGL.h>
#include <OSGGLEXT.h>
#include <OSGAction.h>
#include <OSGDrawAction.h>
#include <OSGRenderAction.h>
#include <OSGIntersectAction.h>
#include <OSGRenderAction.h>
#include <OSGGeoFunctions.h>
#include <OSGViewport.h>
#include <OSGCamera.h>

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

static const u32 sizePool = 3;
#undef GV_COLORPRIMITIVES

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::GeometryClustered

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void GeometryClustered::initMethod (void)
{
    DrawAction::registerEnterDefault(getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE, MaterialDrawablePtr, 
              CNodePtr, Action *>(&MaterialDrawable::drawActionHandler));

    IntersectAction::registerEnterDefault(getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE, GeometryPtr,  
              CNodePtr, Action *>(&Geometry::intersect));

    RenderAction::registerEnterDefault(getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE, MaterialDrawablePtr,  
              CNodePtr, Action *>(&MaterialDrawable::renderActionHandler));
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

GeometryClustered::GeometryClustered(void) :
    Inherited()
{
}

GeometryClustered::GeometryClustered(const GeometryClustered &source) :
    Inherited(source)
{
}

GeometryClustered::~GeometryClustered(void)
{
}

/*----------------------------- class specific ----------------------------*/

void GeometryClustered::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
    if (whichField | NumCellsFieldMask) {
      delete getGrid();
      setGrid(NULL);
      while (getPool().size() > 0) {
	delete getPool(0);
	getPool().erase(getPool().begin());
      }
    }
}

void GeometryClustered::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump GeometryClustered NI" << std::endl;
}

SetUnion& GeometryClustered::getPoolEntry (UInt32 i)
{
#ifdef GV_CLUSTERED_ADAPTIVE
   assert(getPool(0) != NULL);
   return (*getPool(0))[i];
#else
   assert(getPool() != NULL);
   return (*getPool())[i];
#endif
}
const SetUnion& GeometryClustered::getPoolEntry (UInt32 i) const
{
#ifdef GV_CLUSTERED_ADAPTIVE
   assert(getPool(0) != NULL);
   return (*getPool(0))[i];
#else
   assert(getPool() != NULL);
   return (*getPool())[i];
#endif
}
SetUnion& GeometryClustered::getPoolEntry (const Pnt3f& eye, 
					   Real32 minDist, Real32 maxDist, 
					   UInt32 i)
{
#ifdef GV_CLUSTERED_ADAPTIVE
   Int32 pindex;
#if 0
   Real32 dist = (getPositions()->getValue(i)-eye).length();
   pindex = Int32(log10(dist))-Int32(log10(maxDist-minDist));
   if (pindex < 0) {
     pindex = 0;
   } 
   if (dist >= sizePool) {
     pindex = sizePool-1;
   }
#else
   Real32 dist = (getPositions()->getValue(i)-eye).length();
   pindex = Int32(sizePool*(dist-minDist)/(maxDist-minDist));
#endif
   assert(pindex < 3);
   return (*getPool(pindex))[i];
#else
   assert(getPool() != NULL);
   return (*getPool())[i];
#endif
}
const SetUnion& GeometryClustered::getPoolEntry (const Pnt3f& eye, 
						 Real32 minDist, Real32 maxDist, 
						 UInt32 i) const
{
#ifdef GV_CLUSTERED_ADAPTIVE
   Int32 pindex;
#if 0
   Real32 dist = (getPositions()->getValue(i)-eye).length();
   pindex = Int32(log10(dist))-Int32(log10(maxDist-minDist));
   if (pindex < 0) {
     pindex = 0;
   } 
   if (dist >= sizePool) {
     pindex = sizePool-1;
   }
#else
   Real32 dist = (getPositions()->getValue(i)-eye).length();
   pindex = Int32(sizePool*(dist-minDist)/(maxDist-minDist));
#endif
   assert(pindex < 3);
   return (*getPool(pindex))[i];
#else
   assert(getPool() != NULL);
   return (*getPool())[i];
#endif
}


typedef void (OSG_APIENTRY *pumpFunc)(GLubyte * data);
typedef void (OSG_APIENTRY *multiPumpFunc)(GLenum which, GLubyte * data);


// Some helper arrays for function selection
// indexed by data type and dimension

static const int formatBase = GL_BYTE;
static const int numFormats = GL_DOUBLE - GL_BYTE + 1;

static char *formatNames[] =
{   "GL_BYTE", "GL_UNSIGNED_BYTE", "GL_SHORT", "GL_UNSIGNED_SHORT",
    "GL_INT", "GL_UNSIGNED_INT", "GL_FLOAT", "GL_2_BYTES",
    "GL_3_BYTES", "GL_4_BYTES", "GL_DOUBLE"
};

// little helper class for function init

class glextFuncInit
{
    public:

        glextFuncInit(char *name, UInt32 format, UInt32 dim) :
            _name(name), _format(format), _dim(dim) {};

        void init(UInt32 (&extids)[numFormats][4])
        {
            extids[_format - formatBase][_dim] =
                                        Window::registerFunction(_name);
        }

    private:

        char   *_name;
        UInt32  _format;
        UInt32  _dim;
};

static glextFuncInit secondaryColorInitFuncs[8] = {
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3bvEXT",
                  GL_BYTE,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3ubvEXT",
                  GL_UNSIGNED_BYTE,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3svEXT",
                  GL_SHORT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3usvEXT",
                  GL_UNSIGNED_SHORT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3ivEXT",
                  GL_INT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3uivEXT",
                  GL_UNSIGNED_INT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3fvEXT",
                  GL_FLOAT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glSecondaryColor3dvEXT",
                  GL_DOUBLE,
                  3)
};

static glextFuncInit multiTexCoordsInitFuncs[16] = {
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord1svARB",
                  GL_SHORT,
                  1),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord2svARB",
                  GL_SHORT,
                  2),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord3svARB",
                  GL_SHORT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord4svARB",
                  GL_SHORT,
                  4),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord1ivARB",
                  GL_INT,
                  1),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord2ivARB",
                  GL_INT,
                  2),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord3ivARB",
                  GL_INT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord4ivARB",
                  GL_INT,
                  4),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord1fvARB",
                  GL_FLOAT,
                  1),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord2fvARB",
                  GL_FLOAT,
                  2),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord3fvARB",
                  GL_FLOAT,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord4fvARB",
                  GL_FLOAT,
                  4),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord1dvARB",
                  GL_DOUBLE,
                  1),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord2dvARB",
                  GL_DOUBLE,
                  2),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord3dvARB",
                  GL_DOUBLE,
                  3),
    glextFuncInit(OSG_DLSYM_UNDERSCORE"glMultiTexCoord4dvARB",
                  GL_DOUBLE,
                  4)
};

// The real pumping functions

static pumpFunc PositionFuncs[numFormats][4] = {
    { NULL, NULL, NULL, NULL },                           // GL_BYTE
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_BYTE
    { NULL, (pumpFunc)glVertex2sv,
      (pumpFunc)glVertex3sv, (pumpFunc)glVertex4sv },     // GL_SHORT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_SHORT
    { NULL, (pumpFunc)glVertex2iv,
      (pumpFunc)glVertex3iv, (pumpFunc)glVertex4iv },     // GL_INT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_INT
    { NULL, (pumpFunc)glVertex2fv,
      (pumpFunc)glVertex3fv, (pumpFunc)glVertex4fv },     // GL_FLOAT
    { NULL, NULL, NULL, NULL },                           // GL_2_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_3_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_4_BYTES
    { NULL, (pumpFunc)glVertex2dv,
      (pumpFunc)glVertex3dv, (pumpFunc)glVertex4dv },     // GL_DOUBLE
};

static pumpFunc NormalFuncs[numFormats][4] = {
    { NULL, NULL, (pumpFunc)glNormal3sv, NULL },          // GL_BYTE
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_BYTE
    { NULL, NULL, (pumpFunc)glNormal3sv, NULL },          // GL_SHORT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_SHORT
    { NULL, NULL, (pumpFunc)glNormal3iv, NULL },          // GL_INT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_INT
    { NULL, NULL, (pumpFunc)glNormal3fv, NULL },          // GL_FLOAT
    { NULL, NULL, NULL, NULL },                           // GL_2_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_3_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_4_BYTES
    { NULL, NULL, (pumpFunc)glNormal3dv, NULL },          // GL_DOUBLE
};

static pumpFunc ColorFuncs[numFormats][4] = {
    { NULL, NULL,
      (pumpFunc)glColor3bv, (pumpFunc)glColor4bv },       // GL_BYTE
    { NULL, NULL,
      (pumpFunc)glColor3ubv, (pumpFunc)glColor4ubv },     // GL_UNSIGNED_BYTE
    { NULL, NULL,
      (pumpFunc)glColor3sv, (pumpFunc)glColor4sv },       // GL_SHORT
    { NULL, NULL,
      (pumpFunc)glColor3usv, (pumpFunc)glColor4usv },     // GL_UNSIGNED_SHORT
    { NULL, NULL,
      (pumpFunc)glColor3iv, (pumpFunc)glColor4iv },       // GL_INT
    { NULL, NULL,
      (pumpFunc)glColor3uiv, (pumpFunc)glColor4uiv },     // GL_UNSIGNED_INT
    { NULL, NULL,
      (pumpFunc)glColor3fv, (pumpFunc)glColor4fv },       // GL_FLOAT
    { NULL, NULL, NULL, NULL },                           // GL_2_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_3_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_4_BYTES
    { NULL, NULL,
      (pumpFunc)glColor3dv, (pumpFunc)glColor4dv },       // GL_DOUBLE
};

static UInt32 SecColorIDs[numFormats][4];

static pumpFunc TexCoordsFuncs[numFormats][4] = {
    { NULL, NULL, NULL, NULL },                           // GL_BYTE
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_BYTE
    { (pumpFunc)glTexCoord1sv, (pumpFunc)glTexCoord2sv,
      (pumpFunc)glTexCoord3sv, (pumpFunc)glTexCoord4sv }, // GL_SHORT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_SHORT
    { (pumpFunc)glTexCoord1iv, (pumpFunc)glTexCoord2iv,
      (pumpFunc)glTexCoord3iv, (pumpFunc)glTexCoord4iv }, // GL_INT
    { NULL, NULL, NULL, NULL },                           // GL_UNSIGNED_INT
    { (pumpFunc)glTexCoord1fv, (pumpFunc)glTexCoord2fv,
      (pumpFunc)glTexCoord3fv, (pumpFunc)glTexCoord4fv }, // GL_FLOAT
    { NULL, NULL, NULL, NULL },                           // GL_2_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_3_BYTES
    { NULL, NULL, NULL, NULL },                           // GL_4_BYTES
    { (pumpFunc)glTexCoord1dv, (pumpFunc)glTexCoord2dv,
      (pumpFunc)glTexCoord3dv, (pumpFunc)glTexCoord4dv }, // GL_DOUBLE
};

static UInt32 TexCoords1IDs[numFormats][4];

#define TexCoords2IDs TexCoords1IDs
#define TexCoords3IDs TexCoords1IDs

#define pumpSetup( name, typename, getmethod )                              \
    typename name##Ptr;                                                     \
    GLubyte * name##Data = NULL;                                            \
    UInt32 name##Stride;                                                    \
                                                                            \
    name##Ptr = geo->getmethod();                                           \
    if ( name##Ptr != NullFC )                                              \
    {                                                                       \
        name##Data = name##Ptr->getData();                                  \
        if ( ! ( name##Stride = name##Ptr->getStride() ) )                  \
            name##Stride = name##Ptr->getFormatSize() *                     \
            name##Ptr->getDimension();                                      \
    }

#define pumpInternalSetup( name, typename, getmethod, mandatory )           \
    GLubyte * name##Data;                                                   \
    UInt32 name##Stride;                                                    \
    UInt32 name##Ind = 0;                                                   \
    typename name##Ptr;                                                     \
                                                                            \
    name##Ptr = geo->getmethod();                                           \
    if(mandatory && name##Ptr == NullFC)                                    \
    {                                                                       \
        SWARNING << "masterPump: Geometry " << geo << " has no "            \
                 << #name << "s!" << std::endl;                             \
        return;                                                             \
    }                                                                       \
    else if(name##Ptr != NullFC)                                            \
    {                                                                       \
        name##Data = name##Ptr->getData();                                  \
        if(!(name##Stride = name##Ptr->getStride()))                        \
            name##Stride =  name##Ptr->getFormatSize() *                    \
                            name##Ptr->getDimension();                      \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        name##Data = NULL;                                                  \
        name##Stride = 0;                                                   \
    }

#define pumpGLSetup( name, typename, getmethod )                            \
    GLubyte * name##Data;                                                   \
    UInt32 name##Stride;                                                    \
    UInt32 name##Ind = 0;                                                   \
    typename name##Ptr;                                                     \
    pumpFunc name##Func;                                                    \
                                                                            \
    name##Ptr = geo->getmethod();                                           \
    if(name##Ptr != NullFC)                                                 \
    {                                                                       \
        name##Data = name##Ptr->getData();                                  \
        if(!(name##Stride = name##Ptr->getStride()))                        \
            name##Stride = name##Ptr->getFormatSize() *                     \
            name##Ptr->getDimension();                                      \
        if(!(name##Func = name##Funcs[name##Ptr->getFormat() -              \
                                            formatBase]                     \
                                     [name##Ptr->getDimension() - 1]        \
          ) )                                                               \
        {                                                                   \
            SWARNING << "masterPump: Geometry " << geo << " has illegal "   \
                     << #name << "s: " << name##Ptr->getDimension()         \
                     << "D " << formatNames[ name##Ptr->getFormat() -       \
                                            formatBase ]                    \
                     << "!" << std::endl;                                   \
            return;                                                         \
        }                                                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        name##Data = NULL;                                                  \
        name##Func = NULL;                                                  \
        name##Stride = 0;                                                   \
    }

#define pumpGLExtSetup( name, typename, getmethod )                         \
    GLubyte * name##Data;                                                   \
    UInt32 name##Stride;                                                    \
    UInt32 name##Ind = 0;                                                   \
    typename name##Ptr;                                                     \
    pumpFunc name##Func;                                                    \
                                                                            \
    name##Ptr = geo->getmethod();                                           \
    if(name##Ptr != NullFC)                                                 \
    {                                                                       \
        name##Data = name##Ptr->getData();                                  \
        if(!(name##Stride = name##Ptr->getStride()))                        \
            name##Stride = name##Ptr->getFormatSize() *                     \
            name##Ptr->getDimension();                                      \
        if(name##IDs[name##Ptr->getFormat() - formatBase]                   \
                    [name##Ptr->getDimension() - 1] ==                      \
                    Window::invalidFunctionID)                              \
        {                                                                   \
            SWARNING << "masterPump: Geometry " << geo << " has illegal "   \
                     << #name << "s: " << name##Ptr->getDimension()         \
                     << "D " << formatNames[ name##Ptr->getFormat() -       \
                                            formatBase ]                    \
                     << "!" << std::endl;                                   \
            return;                                                         \
        }                                                                   \
        if(!(name##Func = (pumpFunc)win->getFunction(name##IDs              \
                                     [name##Ptr->getFormat() - formatBase]  \
                                     [name##Ptr->getDimension() - 1])       \
          ) )                                                               \
        {                                                                   \
            SWARNING << "masterPump: Geometry " << geo << " uses "          \
                     << #name << "s: " << name##Ptr->getDimension()         \
                     << "D " << formatNames[ name##Ptr->getFormat() -       \
                                            formatBase ]                    \
                     << " which are not supported by Window " << win        \
                     <<  "!" << std::endl;                                  \
            return;                                                         \
        }                                                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        name##Data = NULL;                                                  \
        name##Func = NULL;                                                  \
        name##Stride = 0;                                                   \
    }

#define pumpMultiGLExtSetup(name, typename, getmethod)                      \
    GLubyte * name##Data;                                                   \
    UInt32 name##Stride;                                                    \
    UInt32 name##Ind = 0;                                                   \
    typename name##Ptr;                                                     \
    multiPumpFunc name##Func;                                               \
                                                                            \
    name##Ptr = geo->getmethod();                                           \
    if(name##Ptr != NullFC)                                                 \
    {                                                                       \
        name##Data = name##Ptr->getData();                                  \
        if(!(name##Stride = name##Ptr->getStride() ) )                      \
            name##Stride = name##Ptr->getFormatSize() *                     \
            name##Ptr->getDimension();                                      \
        if(name##IDs[name##Ptr->getFormat() - formatBase]                   \
                    [name##Ptr->getDimension() - 1] ==                      \
                    Window::invalidFunctionID)                              \
        {                                                                   \
            SWARNING << "masterPump: Geometry " << geo << "has illegal "    \
                     << #name << "s: " << name##Ptr->getDimension()         \
                     << "D " << formatNames[ name##Ptr->getFormat() -       \
                                            formatBase ]                    \
                     << "!" << std::endl;                                   \
            return;                                                         \
        }                                                                   \
        if(!(name##Func = (multiPumpFunc)win->getFunction(name##IDs         \
                                     [name##Ptr->getFormat() - formatBase]  \
                                     [name##Ptr->getDimension() - 1])       \
          ) )                                                               \
        {                                                                   \
            SWARNING << "masterPump: Geometry " << geo << "uses "           \
                     << #name << "s: " << name##Ptr->getDimension()         \
                     << "D " << formatNames[ name##Ptr->getFormat() -       \
                                            formatBase ]                    \
                     << "which are not supported by Window " << win         \
                     <<  "!" << std::endl;                                  \
            return;                                                         \
        }                                                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        name##Data = NULL;                                                  \
        name##Func = NULL;                                                  \
        name##Stride = 0;                                                   \
    }

static void masterGeoPump (Window* win, Viewport* port, GeometryClustered* geo, 
			   Matrix wcMatrix, const Pnt3f& minBound, const Pnt3f& maxBound)
{
    static const Real32 sqrt2 = 1.0f/sqrt(2.0f);

    CameraPtr cam = port->getCamera();
    wcMatrix.invert();
    wcMatrix.mult(cam->getBeacon()->getToWorld());
    // calc eye point and center point
    Pnt3f  eye (wcMatrix[3][0], wcMatrix[3][1], wcMatrix[3][2]); 
    Vec3f  diff = maxBound-minBound;
    Pnt3f  center = minBound + 0.5f*diff;
    Real32 d = sqrt2*diff.length();
    Real32 minDist = (center-eye).length();
    Real32 maxDist = minDist + d;
    minDist = (minDist > d ? minDist - d : 0);
    SLOG << "distance=[" << minDist << "," << maxDist << "]" << std::endl;
    // Setup: get all the data
    pumpInternalSetup( Type,   GeoPTypesPtr,   getTypes,   true  );
    pumpInternalSetup( Length, GeoPLengthsPtr, getLengths, false );
    pumpInternalSetup( Index,  GeoIndicesPtr,  getIndices, false );

    pumpGLSetup        ( Position,   GeoPositionsPtr, getPositions      );
    pumpGLSetup        ( Color,      GeoColorsPtr,    getColors         );
    pumpGLExtSetup     ( SecColor,   GeoColorsPtr,    getSecondaryColors);
    pumpGLSetup        ( Normal,     GeoNormalsPtr,   getNormals        );
    pumpGLSetup        ( TexCoords,  GeoTexCoordsPtr, getTexCoords      );
    pumpMultiGLExtSetup( TexCoords1, GeoTexCoordsPtr, getTexCoords1     );
    pumpMultiGLExtSetup( TexCoords2, GeoTexCoordsPtr, getTexCoords2     );
    pumpMultiGLExtSetup( TexCoords3, GeoTexCoordsPtr, getTexCoords3     );

    // check if the node is empty
    if (!TypeData || TypePtr->getSize() == 0) {
        return;
    }
    // if it's not empty we need positions
    if (!PositionData) {
        SWARNING << "masterPump: Geometry " << geo << " has no positions!?!"
                 << std::endl;
        return;
    }
    // find the mapping indices
    UInt16 nmappings = geo->getIndexMapping().size();
    Int16 PositionIndex   = -1,
          NormalIndex     = -1,
          ColorIndex      = -1,
          SecColorIndex   = -1,
          TexCoordsIndex  = -1,
          TexCoords1Index = -1,
          TexCoords2Index = -1,
          TexCoords3Index = -1;

    if (nmappings > 0) { // multi-indexed
        PositionIndex   = geo->calcMappingIndex(Geometry::MapPosition      );
        NormalIndex     = geo->calcMappingIndex(Geometry::MapNormal        );
        ColorIndex      = geo->calcMappingIndex(Geometry::MapColor         );
        SecColorIndex   = geo->calcMappingIndex(Geometry::MapSecondaryColor);
        TexCoordsIndex  = geo->calcMappingIndex(Geometry::MapTexCoords     );
        TexCoords1Index = geo->calcMappingIndex(Geometry::MapTexCoords1    );
        TexCoords2Index = geo->calcMappingIndex(Geometry::MapTexCoords2    );
        TexCoords3Index = geo->calcMappingIndex(Geometry::MapTexCoords3    );
    } else if (IndexData) { // single-indexed
        nmappings = 1;
        PositionIndex =
        NormalIndex =
        ColorIndex =
        SecColorIndex =
        TexCoordsIndex =
        TexCoords1Index =
        TexCoords2Index =
        TexCoords3Index = 0;
    }

    // overall color?
    if (ColorData && ColorPtr->getSize() == 1) {
        ColorFunc(ColorData);
    }

    // Length handling. Special case: no length given

    UInt32 lendummy;
    UInt32 LengthSize;

    // no lengths? use all available data for the first type
    if (!LengthData) {
        LengthSize = 1;
        LengthData = (UChar8*) &lendummy;
        if(IndexData) {
            lendummy = IndexPtr->getSize() / nmappings;
        } else {
            lendummy = PositionPtr->getSize();
        }
    } else {
        LengthSize = LengthPtr->getSize();
    }

    bool omit;
    SetUnion* set = NULL;
    UInt32    index;
    UInt32*   vind;
    // store color on entry
    srand(4711);
    GLfloat current[4];
    glGetFloatv(GL_CURRENT_COLOR, current);
    for (LengthInd = 0; LengthInd < LengthSize; LengthInd++) {
#ifdef GV_COLORPRIMITIVES
        glColor3f(GLfloat(rand())/RAND_MAX, 
		  GLfloat(rand())/RAND_MAX, 
		  GLfloat(rand())/RAND_MAX);
#endif
        GLuint type = *(TypeData + TypeInd++ * TypeStride);
	switch (type) {
	case GL_QUADS: 
	   SINFO << "GL_QUADS" << std::endl;
 	   omit = true;
	   glBegin(GL_TRIANGLE_FAN);
	   break;
	case GL_QUAD_STRIP:
	   SINFO << "GL_QUAD_STRIP" << std::endl;
 	   omit = false;
	   glBegin(type);
	   break;
	case GL_TRIANGLES: {
	   SINFO << "GL_TRIANGLES" << std::endl;
	   SetUnion* setArray[3];
	   glBegin(type);
	   for (UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); l+=3) {
	     SetUnion::nextFrame ();
	     // test if triangle collapses then omit OpenGL commands
	     if (IndexData) {
 	        setArray[0] = geo->getPoolEntry(eye,minDist,maxDist,*(UInt32*)(IndexData + IndexStride * (IndexInd + PositionIndex))).getSet(true);
 	        setArray[1] = geo->getPoolEntry(eye,minDist,maxDist,*(UInt32*)(IndexData + IndexStride * (IndexInd + nmappings + PositionIndex))).getSet(true);
 	        setArray[2] = geo->getPoolEntry(eye,minDist,maxDist,*(UInt32*)(IndexData + IndexStride * (IndexInd + nmappings + PositionIndex + nmappings))).getSet(true);
		if (setArray[0] == NULL || setArray[1] == NULL || setArray[2] == NULL) { 
		   IndexInd += 3*nmappings;
		   continue;
		}
	     } else {
 	        setArray[0] = geo->getPoolEntry(eye,minDist,maxDist,index).getSet(true);
 	        setArray[1] = geo->getPoolEntry(eye,minDist,maxDist,index+1).getSet(true);
 	        setArray[2] = geo->getPoolEntry(eye,minDist,maxDist,index+2).getSet(true);
		if (setArray[0] == NULL || setArray[1] == NULL || setArray[2] == NULL) { 
		   index += 3;
		   continue;
		}
	     }
	     for (UInt32 ll=0; ll<3; ++ll) {
	        // perform lookup
	        if (IndexData) {
		   vind = (UInt32*)(IndexData + IndexStride * IndexInd);
		   IndexInd += nmappings;
		   set = setArray[ll];
		   SINFO << vind[PositionIndex] << " replaced by " << set->getIdentifier() << std::endl;
		} else {
		   vind = &index;
		   ++index;
		   set = setArray[ll];
		   SINFO << index << " replaced by " << set->getIdentifier() << std::endl;
		}
		// pump data
		if (ColorData) {
		   ColorFunc(ColorData + ColorStride * vind[ColorIndex]);
		}
		if (SecColorData) {
		   SecColorFunc(SecColorData + SecColorStride * vind[SecColorIndex]);
		}
		if (NormalData) {
		   NormalFunc(NormalData + NormalStride * vind[NormalIndex]);
		}
		if (TexCoordsData) {
		   TexCoordsFunc(TexCoordsData + TexCoordsStride * vind[TexCoordsIndex]);
		}
		if (TexCoords1Data) {
		   TexCoords1Func(GL_TEXTURE1_ARB,
				  TexCoords1Data + TexCoords1Stride * vind[TexCoords1Index]);
		}
		if (TexCoords2Data) {
		   TexCoords2Func(GL_TEXTURE2_ARB,
				  TexCoords2Data + TexCoords2Stride * vind[TexCoords2Index]);
		}
		if (TexCoords3Data) {
		   TexCoords3Func(GL_TEXTURE3_ARB,
				  TexCoords3Data + TexCoords3Stride * vind[TexCoords3Index]);
		}
		PositionFunc(PositionData + PositionStride * set->getIdentifier());
	     }
	   }
	   glEnd();
	   continue;
	}
	case GL_TRIANGLE_STRIP: {
	   SINFO << "GL_TRIANGLE_STRIP" << std::endl;
	   SetUnion::nextFrame ();
	   glBegin(type);
	   for(UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); ++l) {
	     // perform lookup
	     if (IndexData) {
                vind = (UInt32*)(IndexData + IndexStride * IndexInd);
                IndexInd += nmappings;
		// always use the first two vertices
		if (l > 2) {
		  set = geo->getPoolEntry(eye,minDist,maxDist,vind[PositionIndex]).getSet(true);
		} else {
		  set = geo->getPoolEntry(eye,minDist,maxDist,vind[PositionIndex]).getSet(false);
		}
		if (set == NULL) { // omit vertex
		  SINFO << vind[PositionIndex] << " omitted!" << std::endl;
		  continue; 
		}
		SetUnion::nextFrame ();
		SINFO << vind[PositionIndex] << " replaced by " << set->getIdentifier() << std::endl;
	    } else {
	        vind = &index;
                ++index;
		// always use the first two vertices
		if (l > 2) {
		  set = geo->getPoolEntry(eye,minDist,maxDist,index).getSet(true);
		} else {
		  set = geo->getPoolEntry(eye,minDist,maxDist,index).getSet(false);
		}
		if (set == NULL) { // omit vertex
		   SINFO << index << " omitted!" << std::endl;
		   continue; 
		}
		SetUnion::nextFrame ();
		SINFO << index << " replaced by " << set->getIdentifier() << std::endl;
	    }
	    // pump data
	    if (ColorData) {
	      ColorFunc(ColorData + ColorStride * vind[ColorIndex]);
	    }
	    if (SecColorData) {
	      SecColorFunc(SecColorData + SecColorStride * vind[SecColorIndex]);
	    }
	    if (NormalData) {
	      NormalFunc(NormalData + NormalStride * vind[NormalIndex]);
	    }
	    if (TexCoordsData) {
	      TexCoordsFunc(TexCoordsData + TexCoordsStride * vind[TexCoordsIndex]);
	    }
	    if (TexCoords1Data) {
	      TexCoords1Func(GL_TEXTURE1_ARB,
			     TexCoords1Data + TexCoords1Stride * vind[TexCoords1Index]);
	    }
	    if (TexCoords2Data) {
	      TexCoords2Func(GL_TEXTURE2_ARB,
			     TexCoords2Data + TexCoords2Stride * vind[TexCoords2Index]);
	    }
	    if (TexCoords3Data) {
	      TexCoords3Func(GL_TEXTURE3_ARB,
			     TexCoords3Data + TexCoords3Stride * vind[TexCoords3Index]);
	    }
	    PositionFunc(PositionData + PositionStride * set->getIdentifier());
	   }
	   glEnd();
	   continue;
	}
	case GL_TRIANGLE_FAN: {
	   SINFO << "GL_TRIANGLE_FAN" << std::endl;
	   SetUnion::nextFrame ();
	   glBegin(type);
	   for(UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); ++l) {
	      // perform lookup
	      if (IndexData) {
                vind = (UInt32*)(IndexData + IndexStride * IndexInd);
                IndexInd += nmappings;
		set = geo->getPoolEntry(eye,minDist,maxDist,vind[PositionIndex]).getSet(true);
		if (set == NULL) { 
		  SINFO << vind[PositionIndex] << " omitted!" << std::endl;
		  continue; 
		}
		SINFO << vind[PositionIndex] << " replaced by " << set->getIdentifier() << std::endl;
	      } else {
                vind = &index;
                ++index;
		set = geo->getPoolEntry(eye,minDist,maxDist,index).getSet(true);
		if (set == NULL) {
		   SINFO << index << " omitted!" << std::endl;
		   continue; 
		}
		SINFO << index << " replaced by " << set->getIdentifier() << std::endl;
	      }
	      // perform lookup
	      if (ColorData) {
		ColorFunc(ColorData + ColorStride * vind[ColorIndex]);
	      }
	      if (SecColorData) {
		SecColorFunc(SecColorData + SecColorStride * vind[SecColorIndex]);
	      }
	      if (NormalData) {
		NormalFunc(NormalData + NormalStride * vind[NormalIndex]);
	      }
	      if (TexCoordsData) {
		TexCoordsFunc(TexCoordsData + TexCoordsStride * vind[TexCoordsIndex]);
	      }
	      if (TexCoords1Data) {
		TexCoords1Func(GL_TEXTURE1_ARB,
			       TexCoords1Data + TexCoords1Stride * vind[TexCoords1Index]);
	      }
	      if (TexCoords2Data) {
		TexCoords2Func(GL_TEXTURE2_ARB,
			       TexCoords2Data + TexCoords2Stride * vind[TexCoords2Index]);
	      }
	      if (TexCoords3Data) {
		TexCoords3Func(GL_TEXTURE3_ARB,
			       TexCoords3Data + TexCoords3Stride * vind[TexCoords3Index]);
	      }
	      PositionFunc(PositionData + PositionStride * vind[PositionIndex]);
	   }
	   glEnd();
	   continue;
	}
	case GL_POLYGON:
	   SINFO << "GL_POLYGON" << std::endl;
	   SetUnion::nextFrame ();
	   omit = true;
	   glBegin(type);
	   break;
	default:
 	   SINFO << "other " << type << std::endl;
	   SetUnion::nextFrame ();
	   omit = true;
	   glBegin(type);
	   break;
	};
	for(UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); ++l) {
	   // perform lookup
	   if (IndexData) {
                vind = (UInt32*)(IndexData + IndexStride * IndexInd);
                IndexInd += nmappings;
		set = geo->getPoolEntry(eye,minDist,maxDist,vind[PositionIndex]).getSet(omit);
		if (set == NULL) { 
		  SINFO << vind[PositionIndex] << " omitted!" << std::endl;
		  continue; 
		}
		SINFO << vind[PositionIndex] << " replaced by " << set->getIdentifier() << std::endl;
	   } else {
                vind = &index;
                ++index;
		set = geo->getPoolEntry(eye,minDist,maxDist,index).getSet(omit);
		if (set == NULL) {
		   SINFO << index << " omitted!" << std::endl;
		   continue; 
		}
		SINFO << index << " replaced by " << set->getIdentifier() << std::endl;
	   }
	   // pump data
	   if (ColorData) {
	      ColorFunc(ColorData + ColorStride * vind[ColorIndex]);
	   }
	   if (SecColorData) {
	      SecColorFunc(SecColorData + SecColorStride * vind[SecColorIndex]);
	   }
	   if (NormalData) {
	      NormalFunc(NormalData + NormalStride * vind[NormalIndex]);
	   }
	   if (TexCoordsData) {
	      TexCoordsFunc(TexCoordsData + TexCoordsStride * vind[TexCoordsIndex]);
	   }
	   if (TexCoords1Data) {
	      TexCoords1Func(GL_TEXTURE1_ARB,
			     TexCoords1Data + TexCoords1Stride * vind[TexCoords1Index]);
	   }
	   if (TexCoords2Data) {
	      TexCoords2Func(GL_TEXTURE2_ARB,
			     TexCoords2Data + TexCoords2Stride * vind[TexCoords2Index]);
	   }
	   if (TexCoords3Data) {
	      TexCoords3Func(GL_TEXTURE3_ARB,
			     TexCoords3Data + TexCoords3Stride * vind[TexCoords3Index]);
	   }
	   PositionFunc(PositionData + PositionStride * set->getIdentifier());
	}
        glEnd();
    }
    // restore color on exit
    glColor3fv(current);
}

void GeometryClustered::fillGrid (GeoPositionsPtr pos)
{
#ifndef GV_CLUSTERED_ADAPTIVE
   getPool()->clear();
   getPool()->reserve(pos->getSize());
   for (UInt32 i=0; i<pos->getSize(); ++i) {
      std::vector<SetUnion*>& cell = getGrid()->primitives(pos->getValue(i));
      getPool()->push_back(SetUnion()); SetUnion* set = &(getPool()->back());
      if (cell.size() == 0) {
	 set->init(i, NULL);
 	 cell.push_back(set);
      } else {
	 set->init(i, cell[0]);
 	 cell[0] = set;
      }
   }
   for (i64 i=0; i<getGrid()->getNumVoxels(); ++i) {
      if (getGrid()->getVoxel()[i].size() > 0) {
	 getGrid()->getVoxel()[i][0]->optimize();
      }
   }
#else
   UInt32 i;
   for (i=0; i<sizePool; ++i) {
      getPool(i)->clear();
      getPool(i)->reserve(pos->getSize());
   }
   for (i=0; i<pos->getSize(); ++i) {
      SetUnionGrid::CellType* intern = getGrid()->getLeafCell(pos->getValue(i));
      UInt32 j=0; 
      while (j<3 && intern != NULL) {
	 getPool(j)->push_back(SetUnion()); SetUnion* set = &(getPool(j)->back());
	 std::vector<SetUnion*>& cell = intern->getContainer();
	 if (cell.size() == 0) {
	    set->init(i, NULL);
	    cell.push_back(set);
	 } else {
	    set->init(i, cell[0]);
	    cell[0] = set;
	 }
	 intern = intern->getParent(); ++j;
      } 
   }
   for (u32 xCode=0; xCode<getGrid()->getMaxCode(); ++xCode) {
      for (u32 yCode=0; yCode<getGrid()->getMaxCode(); ++yCode) {
	 for (u32 zCode=0; zCode<getGrid()->getMaxCode(); ++zCode) {
	   SetUnionGrid::CellType* intern = getGrid()->getCell(xCode, yCode, zCode);
	   std::vector<SetUnion*>& cell = intern->getContainer();
	   if (cell.size() > 0) {
	      cell[0]->optimize();
	   }
	 }
      }
   }
#endif
}

Action::ResultE GeometryClustered::drawPrimitives (DrawActionBase* action)
{
   // call the pump
   NodePtr node = action->getActNode();

   Pnt3f minBound, maxBound;
   node->getVolume().getBounds(minBound, maxBound);

   // create datastructures
#ifndef GV_CLUSTERED_ADAPTIVE
   if (getGrid() == NULL) {
      setGrid(new SetUnionGrid());
   }
   if (getPool() == NULL) {
      setPool(new SetUnionPool());
   }
   // init grid
   SetUnion::firstFrame ();
   getGrid()->init(K6Dop(minBound.getValues(), maxBound.getValues()),
		   getNumCells(), 
		   RegularGridBase::MinVoxelsPerDim);
   fillGrid(getPositions());
   SLOG << "RegularGrid created: " << std::endl;
#else
   while (getPool().size() < sizePool) {
      getPool().addValue(new SetUnionPool());
   }
   if (getGrid() == NULL) {
      setGrid(new SetUnionGrid());
      // init grid
      SetUnion::firstFrame ();
      getGrid()->init(K6Dop(minBound.getValues(), maxBound.getValues()),
		      getNumCells(), 
		      RegularGridBase::MaxVoxels);
      fillGrid(getPositions());
      SLOG << "RegularGrid created: " << std::endl;
   }
#endif

   masterGeoPump(action->getWindow(), action->getViewport(), this, 
		 node->getToWorld(), minBound, maxBound);

   StatCollector *coll = action->getStatistics();
   if(coll != NULL) {
      StatIntElem *el = coll->getElem(Drawable::statNTriangles,false);
      if (el != NULL) {
	 GeometryPtr geo(this);
	 UInt32 ntri,nl,np,is;
            
	 calcPrimitiveCount(geo, ntri, nl, np);
	 el->add(ntri);
	 coll->getElem(Drawable::statNLines)->add(nl);
	 coll->getElem(Drawable::statNLines)->add(np);
	 
	 if(getIndices() == NullFC) {
	    if(getPositions() != NullFC) {
	       is = getPositions()->getSize();
	    } else {
	       is = 0;
	    }
	 } else {
	    is = getIndexMapping().size();
	    is = getIndices()->getSize() /(is ? is : 1);
	 }
	 coll->getElem(Drawable::statNVertices)->add(is);
      }
   }
    
   return Action::Continue;
}

/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGGeometryClustered.cpp,v 1.4 2004/03/12 13:37:26 fuenfzig Exp $";
    static Char8 cvsid_hpp       [] = OSGGEOMETRYCLUSTEREDBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGGEOMETRYCLUSTEREDBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGGEOMETRYCLUSTEREDFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

