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

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

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
}

void GeometryClustered::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump GeometryClustered NI" << std::endl;
}

/*! Element datastructure for separating the Positions field into clusters. 
    The elements are organized into a union-find datastructure. After calling 
    optimize() the query time is constant!
 */
class SetUnion : public Adapter
{
public:
   /*! Construct empty datastructure. */
   inline SetUnion  ();
   /*! Construct new element in the set pred. */
   inline SetUnion  (UInt32 id, SetUnion* pred);
   /*! Init element in the set pred. */
   inline void init (UInt32 id, SetUnion* pred);

   /*! Identifier of this element. */
   inline UInt32    getIdentifier  () const;
   /*! Predecessor of this element. */
   inline SetUnion* getPredecessor () const;
   /*! Change the predecessor of this element. Changes the set membership
       implicitly! */
   inline void      setPredecessor (SetUnion* pred);
   /*! Set membership of this element. */
   inline SetUnion* getSetInternal () const;
   /*! Set membership of this element. Same as getSetInternal, 
       if omit==false. Otherwise the timestamp of this element is compared
       to the current timestamp and the set memebership is returned only
       if the timestamps are different. The timestamp of this element is set
       to the current timestamp. */
   inline SetUnion* getSet (bool omit=false);
   /*! Count size of the set this element belongs to. */
   inline UInt32    getSize () const;
   /*! Change the set membership of this element. Not implemented yet! */
   inline void      chooseRepresent (const GeoPositionsPtr& pos);
   /*! Perform path shortening. */
   inline void      optimize ();

   /*! Init timestamps. */
   static inline void   firstFrame ();
   /*! Switch to next timestamp. */
   static inline void   nextFrame ();
 
   /*! Identifier of this adapter class. */
   static inline unsigned getAdapterId();

private:
   static UInt32 s_currentStamp;
   UInt32    m_stamp;
   UInt32    m_id;
   SetUnion* m_pred;
};
UInt32 SetUnion::s_currentStamp = 0;
inline void   SetUnion::firstFrame ()
{
   s_currentStamp = 0;
}
inline void   SetUnion::nextFrame ()
{
   ++s_currentStamp;
}
inline unsigned SetUnion::getAdapterId ()
{
   static unsigned id = getNextId();
   return id;
}
inline SetUnion::SetUnion()
  : m_id(0), m_stamp(0), m_pred(NULL)
{
}
inline UInt32 SetUnion::getSize () const
{
   UInt32 i = 0;
   const SetUnion* set  = this;
   const SetUnion* pred = set->getPredecessor();
   // search for set 
   while (pred != NULL) {
      ++i;
      set  = pred;
      pred = set->getPredecessor();
   }
   return i;
}
inline void SetUnion::init (UInt32 id, SetUnion* pred)
{
   m_id = id; 
   m_stamp = 0;
   m_pred = pred;
}
inline SetUnion::SetUnion(UInt32 id, SetUnion* pred)
{
   init(id, pred);
}
inline UInt32    SetUnion::getIdentifier  () const
{
   return m_id;
}
inline SetUnion* SetUnion::getPredecessor () const
{
   return m_pred;
}
inline void SetUnion::setPredecessor (SetUnion* pred)
{
   m_pred = pred;
}
inline SetUnion* SetUnion::getSetInternal () const
{
   SetUnion* set  = (SetUnion*)this;
   SetUnion* pred = set->getPredecessor();
   // search for set 
   while (pred != NULL) {
      set  = pred;
      pred = set->getPredecessor();
   }
   return set;
}
inline SetUnion* SetUnion::getSet (bool omit)
{
   SetUnion* set = getSetInternal();
   if (!omit) {
      return set;
   }
   if (set->m_stamp != s_currentStamp) {
      // apply current stamp
      set->m_stamp = s_currentStamp;
      return set;
   }
   return NULL;
}
inline void SetUnion::optimize ()
{
   SetUnion* set  = getSetInternal();
   // path shortening
   SetUnion* iter = this;
   SetUnion* pred;
   while (iter != set) {
      // shortcut to set object
      pred = iter->getPredecessor();
      iter->setPredecessor(set);
      iter = pred;
   }
}
inline void SetUnion::chooseRepresent (const GeoPositionsPtr&)
{
   // do nothing at first!
}



#if 0
class PositionsGrid
{
public:
   inline PositionsGrid (Real32 cellSize, 
		  const Pnt3f& minBound, const Pnt3f& maxBound);

   inline void                   create    (GeoPositionsPtr pos);

   inline std::vector<SetUnion>& createAll (UInt32 num);
   inline void                   clearAll();

   inline void                   map (const Pnt3f& p, 
			       UInt32& x, UInt32& y, UInt32& z);
   inline SetUnion*              query (UInt32 id);
   inline SetUnion*              query (const Pnt3f& p);

   inline Real32                 getSize () const;

protected:
   std::vector<std::vector<std::vector<SetUnion*> > > m_grid;
   std::vector<SetUnion>                              m_pool;
   std::vector<SetUnion*>                             m_current;
   Real32 m_cellSize;
   Pnt3f  m_minBound;
   Pnt3f  m_maxBound;
   UInt32 m_numX, m_numY, m_numZ;
   Real32 m_avg;
};
inline std::vector<SetUnion>& PositionsGrid::createAll (UInt32 num)
{
   m_pool.reserve(num);
   return m_pool;
}
inline void                  PositionsGrid::clearAll()
{
   m_pool.clear();
   m_current.clear();
}
inline   PositionsGrid::PositionsGrid (Real32 cellSize,
				       const Pnt3f& minBound, const Pnt3f& maxBound)
  : m_cellSize(cellSize), m_minBound(minBound), m_maxBound(maxBound), m_avg(0)
{
   m_numX = ceil((m_maxBound[0]-m_minBound[0])/m_cellSize+0.5);
   m_numY = ceil((m_maxBound[1]-m_minBound[1])/m_cellSize+0.5);
   m_numZ = ceil((m_maxBound[2]-m_minBound[2])/m_cellSize+0.5);

   m_grid.resize(m_numX);
   for (UInt32 x=0; x<m_numX; ++x) {
      std::vector<std::vector<SetUnion*> >& gridX = m_grid[x];
      gridX.resize(m_numY);
      for (UInt32 y=0; y<m_numY; ++y) {
	 gridX[y].resize(m_numZ);
	 // initialize with NULL
	 std::fill(gridX[y].begin(), gridX[y].end(), NULL);
      }
   }
}
inline Real32 PositionsGrid::getSize () const
{
   return m_avg;
}
inline void PositionsGrid::map (const Pnt3f& p, 
				UInt32& x, UInt32& y, UInt32& z)
{
   x = (p[0]-m_minBound[0])/m_cellSize;
   y = (p[1]-m_minBound[1])/m_cellSize;
   z = (p[2]-m_minBound[2])/m_cellSize;
}
inline SetUnion* PositionsGrid::query (UInt32 id)
{
   return m_pool.begin() + id;
}
inline SetUnion* PositionsGrid::query (const Pnt3f& p)
{
   UInt32 x, y, z; map (p, x, y, z);
   return m_grid[x][y][z];
}
inline void                  PositionsGrid::create    (GeoPositionsPtr pos)
{
   UInt32 x, y, z;
   std::vector<SetUnion>& all = createAll(pos->getSize());
   for (UInt32 i=0; i<pos->getSize(); ++i) {
      all.push_back(SetUnion()); SetUnion* set = &(all.back());
      map(pos->getValue(i), x, y, z);
      if (m_grid[x][y][z] == NULL) {
	 set->init(i, NULL);
 	 m_grid[x][y][z] = set;
	 m_current.push_back(set);
      } else {
	 set->init(i, m_grid[x][y][z]);
 	 m_grid[x][y][z] = set;
      }
   }
   for (UInt32 x=0; x<m_numX; ++x) {
      std::vector<std::vector<SetUnion*> >& gridX = m_grid[x];
      for (UInt32 y=0; y<m_numY; ++y) {
	 std::vector<SetUnion*>& gridY = gridX[y];
	 for (UInt32 z=0; z<m_numZ; ++z) {
	    if (gridY[z] != NULL) {
	       m_avg += gridY[z]->getSize();
	       gridY[z]->optimize();
	    }
	 }
      }
   }
   m_avg /= Real32(m_numX)*Real32(m_numY)*Real32(m_numZ);
}
#endif



RegularGrid<SetUnion>& GeometryClustered::getGrid ()
{
   return m_grid;
}
const RegularGrid<SetUnion>& GeometryClustered::getGrid () const
{
   return m_grid;
}

std::vector<SetUnion>& GeometryClustered::getPool ()
{
   return m_pool;
}
const std::vector<SetUnion>& GeometryClustered::getPool () const
{
   return m_pool;
}

SetUnion& GeometryClustered::getPool (UInt32 i)
{
   return m_pool[i];
}
const SetUnion& GeometryClustered::getPool (UInt32 i) const
{
   return m_pool[i];
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

static void masterGeoPump (Window* win, GeometryClustered* geo)
{
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
    for (LengthInd = 0; LengthInd < LengthSize; LengthInd++) {
        GLuint type = *(TypeData + TypeInd++ * TypeStride);
	switch (type) {
	case GL_QUADS: 
	   SLOG << "GL_QUADS" << std::endl;
 	   omit = true;
	   glBegin(GL_TRIANGLE_FAN);
	   break;
	case GL_QUAD_STRIP:
	   SLOG << "GL_QUAD_STRIP" << std::endl;
 	   omit = false;
	   glBegin(type);
	   break;
	case GL_TRIANGLES: {
	   SLOG << "GL_TRIANGLES" << std::endl;
	   SetUnion* setArray[3];
	   glBegin(type);
	   for (UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); l+=3) {
	     SetUnion::nextFrame ();
	     // test if triangle collapses then omit OpenGL commands
	     if (IndexData) {
 	        setArray[0] = geo->getPool()[*(UInt32*)(IndexData + IndexStride * (IndexInd + PositionIndex))].getSet(true);
 	        setArray[1] = geo->getPool()[*(UInt32*)(IndexData + IndexStride * (IndexInd + nmappings + PositionIndex))].getSet(true);
 	        setArray[2] = geo->getPool()[*(UInt32*)(IndexData + IndexStride * (IndexInd + nmappings + PositionIndex + nmappings))].getSet(true);
		if (setArray[0] == NULL || setArray[1] == NULL || setArray[2] == NULL) { 
		   IndexInd += 3*nmappings;
		   continue;
		}
	     } else {
 	        setArray[0] = geo->getPool()[index].getSet(true);
 	        setArray[1] = geo->getPool()[index+1].getSet(true);
 	        setArray[2] = geo->getPool()[index+2].getSet(true);
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
	   SLOG << "GL_TRIANGLE_STRIP" << std::endl;
	   SetUnion::nextFrame ();
	   glBegin(type);
	   for(UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); ++l) {
	     // perform lookup
	     if (IndexData) {
                vind = (UInt32*)(IndexData + IndexStride * IndexInd);
                IndexInd += nmappings;
		// always use the first two vertices
		if (l > 2) {
		  set = geo->getPool()[vind[PositionIndex]].getSet(true);
		} else {
		  set = geo->getPool()[vind[PositionIndex]].getSet(false);
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
		  set = geo->getPool()[index].getSet(true);
		} else {
		  set = geo->getPool()[index].getSet(false);
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
	   SLOG << "GL_TRIANGLE_FAN" << std::endl;
	   SetUnion::nextFrame ();
	   glBegin(type);
	   for(UInt32 l=0; l<*(UInt32*)(LengthData + LengthInd * LengthStride); ++l) {
	      // perform lookup
	      if (IndexData) {
                vind = (UInt32*)(IndexData + IndexStride * IndexInd);
                IndexInd += nmappings;
		set = geo->getPool()[vind[PositionIndex]].getSet(true);
		if (set == NULL) { 
		  SINFO << vind[PositionIndex] << " omitted!" << std::endl;
		  continue; 
		}
		SINFO << vind[PositionIndex] << " replaced by " << set->getIdentifier() << std::endl;
	      } else {
                vind = &index;
                ++index;
		set = geo->getPool()[index].getSet(true);
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
	   SLOG << "GL_POLYGON" << std::endl;
	   SetUnion::nextFrame ();
	   omit = true;
	   glBegin(type);
	   break;
	default:
 	   SLOG << "other " << type << std::endl;
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
		set = geo->getPool()[vind[PositionIndex]].getSet(omit);
		if (set == NULL) { 
		  SINFO << vind[PositionIndex] << " omitted!" << std::endl;
		  continue; 
		}
		SINFO << vind[PositionIndex] << " replaced by " << set->getIdentifier() << std::endl;
	   } else {
                vind = &index;
                ++index;
		set = geo->getPool()[index].getSet(omit);
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
}

void GeometryClustered::fillGrid (GeoPositionsPtr pos)
{
   m_pool.clear();
   m_pool.reserve(pos->getSize());
   for (UInt32 i=0; i<pos->getSize(); ++i) {
      std::vector<SetUnion*>& cell = m_grid.primitives(pos->getValue(i));
      m_pool.push_back(SetUnion()); SetUnion* set = &(m_pool.back());
      if (cell.size() == 0) {
	 set->init(i, NULL);
 	 cell.push_back(set);
      } else {
	 set->init(i, cell[0]);
 	 cell[0] = set;
      }
   }
   for (i64 i=0; i<m_grid.getNumVoxels(); ++i) {
      if (m_grid.getVoxel()[i].size() > 0) {
	 m_grid.getVoxel()[i][0]->optimize();
      }
   }
}

Action::ResultE GeometryClustered::drawPrimitives (DrawActionBase* action)
{
   // call the pump
   NodePtr node = action->getActNode();

   Pnt3f minBound, maxBound;
   node->getVolume().getBounds(minBound, maxBound);
   Real32 d = osgMax(osgMax(maxBound[0]-minBound[0], maxBound[1]-minBound[1]), maxBound[2]-minBound[2]);

   SetUnion::firstFrame ();
   m_grid.init(K6Dop(minBound.getValues(), maxBound.getValues()),
	       getNumCells(), 
	       RegularGridBase::MinVoxelsPerDim);
   fillGrid(getPositions());
   SLOG << "RegularGrid created: " << std::endl;

   masterGeoPump(action->getWindow(), this);

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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGGeometryClustered.cpp,v 1.3 2003/09/19 21:56:27 fuenfzig Exp $";
    static Char8 cvsid_hpp       [] = OSGGEOMETRYCLUSTEREDBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGGEOMETRYCLUSTEREDBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGGEOMETRYCLUSTEREDFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

