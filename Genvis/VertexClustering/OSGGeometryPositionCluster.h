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

#ifndef _OSGGEOMETRYPOSITIONCLUSTER_H_
#define _OSGGEOMETRYPOSITIONCLUSTER_H_
#ifdef __sgi
#pragma once
#endif

#include <vector>
#include "OSGConfig.h"
#include "OSGBaseTypes.h"
#include "OSGSField.h"
#include "OSGMField.h"
#include "OSGGeoPropPtrs.h"
#include "OSGGeoPositionsFields.h"

// switch to use adaptive grid (octree) instead of regular grid
#define GV_CLUSTERED_ADAPTIVE

#include "OSGGVBase.h"
#ifndef GV_CLUSTERED_ADAPTIVE
#include "OSGGVRegularGrid.h"
#else
#include "OSGGVAdaptiveGrid.h"
#endif

OSG_BEGIN_NAMESPACE

class OSG_GENVISLIB_DLLMAPPING CellDataInternal
{
public:
   /*! Construct empty datastructure. */
   inline CellDataInternal ();

   inline UInt32       getNumPoints () const;
   inline void         setNumPoints (UInt32 num);
   inline Vec3f&       getNormal ();
   inline const Vec3f& getNormal () const;
   inline Pnt3f&       getPointRep ();
   inline const Pnt3f& getPointRep () const;
   inline Matrix&       getQuadric ();
   inline const Matrix& getQuadric () const;
   inline void         clear ();

private:
   UInt32 m_num;
   Vec3f  m_normal;
   Pnt3f  m_rep;
   static Matrix m_quad;
};

inline void         CellDataInternal::clear ()
{
   m_num = 0;
   m_normal.setValue(Vec3f::Null);
   m_rep.setValue(Pnt3f::Null);
#if 0
   m_quad.setValue(0, 0, 0, 0,
		   0, 0, 0, 0,
		   0, 0, 0, 0,
		   0, 0, 0, 0);
#endif
}
inline CellDataInternal::CellDataInternal ()
{
   clear();
}
inline UInt32       CellDataInternal::getNumPoints () const
{
   return m_num;
}
inline void         CellDataInternal::setNumPoints (UInt32 num)
{
   m_num = num;
}
inline Vec3f&      CellDataInternal::getNormal () 
{
   return m_normal;
}
inline const Vec3f&         CellDataInternal::getNormal () const
{
   return m_normal;
}
#if 0
inline Vec3f CellDataInternal::getNormal () const
{
   Real32 factor = sqrt(m_quad[0][0]);
   Vec3f norm(factor, m_quad[0][1]/factor, m_quad[0][2]/factor);
   //norm.normalize();
   return norm;
}
#endif
inline Pnt3f&       CellDataInternal::getPointRep ()
{
   return m_rep;
}
inline const Pnt3f& CellDataInternal::getPointRep () const
{
   return m_rep;
}
inline Matrix&       CellDataInternal::getQuadric ()
{
   return m_quad;
}
inline const Matrix& CellDataInternal::getQuadric () const
{
   return m_quad;
}


#if 0
/*! 
 */
template <class ADAPTER>
class OSG_GENVISLIB_DLLMAPPING CellDataTemplate
{
public:
   /*! Construct empty datastructure. */
   inline CellDataTemplate ();

   inline UInt32       getNumPoints () const;
   inline void         setNumPoints (UInt32 num);
   inline Pnt3f&       getPointRep ();
   inline const Pnt3f& getPointRep () const;
   inline void         clear ();

   /*! Init timestamps. */
   static inline void   firstFrame ();
   /*! Switch to next timestamp. */
   static inline void   nextFrame ();

private:
   static UInt32 s_currentStamp;
   UInt32 m_stamp;
   UInt32 m_num;
   Pnt3f  m_rep;
};
typedef CellDataTemplate<DummyAdapter> CellData;

template <class ADAPTER>
inline CellDataTemplate<ADAPTER>::CellDataTemplate ()
  : m_stamp(0), m_num(0)
{
}
template <class ADAPTER>
inline void         CellDataTemplate<ADAPTER>::clear ()
{
   m_num = 0;
   m_rep.setValue(Pnt3f::Null);
}
template <class ADAPTER>
inline UInt32       CellDataTemplate<ADAPTER>::getNumPoints () const
{
   return m_num;
}
template <class ADAPTER>
inline void         CellDataTemplate<ADAPTER>::setNumPoints (UInt32 num)
{
   m_num = num;
}
template <class ADAPTER>
inline Pnt3f&       CellDataTemplate<ADAPTER>::getPointRep ()
{
   return m_rep;
}
template <class ADAPTER>
inline const Pnt3f& CellDataTemplate<ADAPTER>::getPointRep () const
{
   return m_rep;
}

template <class ADAPTER>
inline void   CellDataTemplate<ADAPTER>::firstFrame ()
{
   s_currentStamp = 0;
}
template <class ADAPTER>
inline void   CellDataTemplate<ADAPTER>::nextFrame ()
{
   ++s_currentStamp;
}
#else
class CellDataInternal;
/*! 
 */
template <class ADAPTER>
class OSG_GENVISLIB_DLLMAPPING CellDataTemplate
{
public:
   inline CellDataTemplate () {
      m_data = NULL;
   }
   inline void clear () {
      m_data = NULL;
      for (std::vector<ADAPTER*>::const_iterator it=s_factory.begin();
	   it != s_factory.end();
	   ++it) {
	 delete (*it);
      }
      s_factory.clear();
   }
   inline void createAdapter () const {
      s_factory.push_back(new ADAPTER());
      m_data = s_factory.back();
   }
   inline ADAPTER* operator-> () {
      if (m_data == NULL) {
	 createAdapter();
      }
      return m_data;
   }
   inline const ADAPTER* operator-> () const {
      if (m_data == NULL) {
	 createAdapter();
      }
      return m_data;
   }
   inline ADAPTER& operator* () {
      if (m_data == NULL) {
	 createAdapter();
      }
      return *m_data;
   }
   inline const ADAPTER& operator* () const {
      if (m_data == NULL) {
	 createAdapter();
      }
      return *m_data;
   }

   static inline void   firstFrame () {
   }
   static inline void   nextFrame () {
      s_query.reserve(8);
      s_query.clear();
   }
   inline bool          isValid () const {
      if (std::find(s_query.begin(), s_query.end(), this)==s_query.end()) {
	 s_query.push_back(this);
         return true;
      }
      return false;
      //return (m_stamp == s_currentStamp);
   }

protected:
   static std::vector<const CellDataTemplate<ADAPTER>*> s_query;
   static std::vector<ADAPTER*>                         s_factory;
   mutable ADAPTER* m_data;
};
typedef CellDataTemplate<CellDataInternal> CellData;

#endif

#if !defined(OSG_DO_DOC) || (OSG_DOC_LEVEL >= 3)

/*! \ingroup FieldLib
 */
#if !defined(OSG_DOC_DEV_TRAITS)
/*! \hideinhierarchy */
#endif

#ifdef GV_CLUSTERED_ADAPTIVE
typedef genvis::AdaptiveGrid<CellDataInternal, CellData>  SetUnionGrid;
typedef genvis::AdaptiveGrid<CellDataInternal, CellData>* SetUnionGridP;
#else
typedef genvis::RegularGrid<CellDataInternal, CellData>   SetUnionGrid;
typedef genvis::RegularGrid<CellDataInternal, CellData>*  SetUnionGridP;
#endif

template <>
struct FieldDataTraits<SetUnionGridP> : 
    public FieldTraitsIntegralRecurseMapper<SetUnionGridP>
{
    static DataType             _type;                       

    enum                        { StringConvertable = 0x00 };
    enum                        { bHasParent        = 0x01 };

    static DataType& getType (void) { return _type;        }

    static char* getName(void) { 
       return "SetUnionGridP"; 
    }
    static char* getSName(void) { 
       return "SFSetUnionGridP"; 
    }
    static char* getMName(void) { 
       return "MFSetUnionGridP"; 
    }

    static UInt32 getBinSize (const SetUnionGridP&) {
       return sizeof(SetUnionGridP);
    }
    static UInt32    getBinSize (const SetUnionGridP*,
				 UInt32  uiNumObjects) {
        return sizeof(SetUnionGridP) * 3 * uiNumObjects;
    }
    static void   copyToBin  (BinaryDataHandler&, const SetUnionGridP&) {
    }
    static void   copyFromBin(BinaryDataHandler&, SetUnionGridP&) {
    }
    static void copyToBin(      BinaryDataHandler &,
                                const SetUnionGridP*,
                                UInt32             ) {
    }
    
    static void copyFromBin(    BinaryDataHandler &,
                                SetUnionGridP*,
                                UInt32             ) {
    }
};

#if !defined(OSG_DOC_DEV_TRAITS)
/*! \class FieldTraitsRecurseBase<SetUnionGridP> */
/*! \hideinhierarchy                               */
#endif

#endif // !defined(OSG_DO_DOC) || (OSG_DOC_LEVEL >= 3)

/*! \brief SetUnionGridP fields
*/

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_FIELD_TYPEDEFS) 
/*! \ingroup GrpBaseFieldSingle */

typedef SField<SetUnionGridP> SFSetUnionGridP;
#endif

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_FIELD_TYPEDEFS)
/*! \ingroup GrpBaseFieldSingle */

typedef MField<SetUnionGridP> MFSetUnionGridP;
#endif

#ifndef OSG_COMPILESETUNIONPFIELDSINST
OSG_DLLEXPORT_DECL1(SField, SetUnionGridP, OSG_GENVISLIB_DLLTMPLMAPPING)
OSG_DLLEXPORT_DECL1(MField, SetUnionGridP, OSG_GENVISLIB_DLLTMPLMAPPING)
#endif

OSG_END_NAMESPACE

#define OSGGEOMETRYPOSITIONCLUSTER_HEADER_CVSID "@(#)$Id: OSGGeometryPositionCluster.h,v 1.2 2004/12/20 15:54:30 fuenfzig Exp $"

#endif /* _OSGGEOMETRYPOSITIONCLUSTER_H_ */
