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

#include "OSGGVBase.h"
#define GV_CLUSTERED_ADAPTIVE
#ifndef GV_CLUSTERED_ADAPTIVE
#include "OSGGVRegularGrid.h"
#else
#include "OSGGVAdaptiveGrid.h"
#endif

OSG_BEGIN_NAMESPACE

/*! Element datastructure for separating the Positions field into clusters. 
    The elements are organized into a union-find datastructure. After calling 
    optimize() the query time is constant!
 */
class OSG_GENVISLIB_DLLMAPPING SetUnion : public genvis::Adapter
{
public:
   /*! Construct empty datastructure. */
   SetUnion  ();
   /*! Construct new element in the set pred. */
   SetUnion  (UInt32 id, SetUnion* pred);
   /*! Init element in the set pred. */
   void init (UInt32 id, SetUnion* pred);

   /*! Identifier of this element. */
   UInt32    getIdentifier  () const;
   /*! Predecessor of this element. */
   SetUnion* getPredecessor () const;
   /*! Change the predecessor of this element. Changes the set membership
       implicitly! */
   void      setPredecessor (SetUnion* pred);
   /*! Set membership of this element. */
   SetUnion* getSetInternal () const;
   /*! Set membership of this element. Same as getSetInternal, 
       if omit==false. Otherwise the timestamp of this element is compared
       to the current timestamp and the set memebership is returned only
       if the timestamps are different. The timestamp of this element is set
       to the current timestamp. */
   SetUnion* getSet (bool omit=false);
   /*! Count size of the set this element belongs to. */
   UInt32    getSize () const;
   /*! Change the set membership of this element. Not implemented yet! */
   void      chooseRepresent (const GeoPositionsPtr& pos);
   /*! Perform path shortening. */
   void      optimize ();

   /*! Init timestamps. */
   static void   firstFrame ();
   /*! Switch to next timestamp. */
   static void   nextFrame ();
 
   /*! Identifier of this adapter class. */
   static unsigned getAdapterId();

private:
   static UInt32 s_currentStamp;
   UInt32    m_stamp;
   UInt32    m_id;
   SetUnion* m_pred;
};

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
#if 1
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
#else
   return getSetInternal();
#endif
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



#if !defined(OSG_DO_DOC) || (OSG_DOC_LEVEL >= 3)

/*! \ingroup FieldLib
 */
#if !defined(OSG_DOC_DEV_TRAITS)
/*! \hideinhierarchy */
#endif

#ifdef GV_CLUSTERED_ADAPTIVE
typedef genvis::AdaptiveGrid<SetUnion>  SetUnionGrid;
typedef genvis::AdaptiveGrid<SetUnion>* SetUnionGridP;
#else
typedef genvis::RegularGrid<SetUnion>  SetUnionGrid;
typedef genvis::RegularGrid<SetUnion>* SetUnionGridP;
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


#if !defined(OSG_DO_DOC) || (OSG_DOC_LEVEL >= 3)

/*! \ingroup FieldLib
 */
#if !defined(OSG_DOC_DEV_TRAITS)
/*! \hideinhierarchy */
#endif

typedef std::vector<SetUnion>  SetUnionPool;
typedef std::vector<SetUnion>* SetUnionPoolP;

template <>
struct FieldDataTraits<SetUnionPoolP> : 
    public FieldTraitsIntegralRecurseMapper<SetUnionPoolP>
{
    static DataType             _type;                       

    enum                        { StringConvertable = 0x00 };
    enum                        { bHasParent        = 0x01 };

    static DataType& getType (void) { return _type;        }

    static char* getName(void) { 
       return "SetUnionPoolP";
    }
    static char* getSName(void) { 
       return "SFSetUnionPoolP";
    }
    static char* getMName(void) { 
       return "MFSetUnionPoolP";
    }

    static UInt32 getBinSize (const SetUnionPoolP&) {
       return sizeof(SetUnionPoolP);
    }
    static UInt32    getBinSize (const SetUnionPoolP*,
				 UInt32  uiNumObjects) {
        return sizeof(SetUnionPoolP) * 3 * uiNumObjects;
    }
    static void   copyToBin  (BinaryDataHandler&, const SetUnionPoolP&) {
    }
    static void   copyFromBin(BinaryDataHandler&, SetUnionPoolP&) {
    }
    static void copyToBin(      BinaryDataHandler &,
                                const SetUnionPoolP*,
                                UInt32             ) {
    }
    
    static void copyFromBin(    BinaryDataHandler &,
                                SetUnionPoolP*,
                                UInt32             ) {
    }
};

#if !defined(OSG_DOC_DEV_TRAITS)
/*! \class FieldTraitsRecurseBase<SetUnionPoolP> */
/*! \hideinhierarchy                               */
#endif

#endif // !defined(OSG_DO_DOC) || (OSG_DOC_LEVEL >= 3)

/*! \brief SetUnionPoolP fields
*/

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_FIELD_TYPEDEFS) 
/*! \ingroup GrpBaseFieldSingle */

typedef SField<SetUnionPoolP> SFSetUnionPoolP;
#endif
#if !defined(OSG_DO_DOC) || defined(OSG_DOC_FIELD_TYPEDEFS) 
/*! \ingroup GrpBaseFieldSingle */

typedef MField<SetUnionPoolP> MFSetUnionPoolP;
#endif

#ifndef OSG_COMPILESETUNIONPFIELDSINST
OSG_DLLEXPORT_DECL1(SField, SetUnionPoolP, OSG_GENVISLIB_DLLTMPLMAPPING)
OSG_DLLEXPORT_DECL1(MField, SetUnionPoolP, OSG_GENVISLIB_DLLTMPLMAPPING)
#endif

OSG_END_NAMESPACE

#define OSGGEOMETRYPOSITIONCLUSTER_HEADER_CVSID "@(#)$Id: OSGGeometryPositionCluster.h,v 1.1 2004/03/12 13:37:26 fuenfzig Exp $"

#endif /* _OSGGEOMETRYPOSITIONCLUSTER_H_ */
