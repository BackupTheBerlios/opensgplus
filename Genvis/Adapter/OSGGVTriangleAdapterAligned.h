//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//                                License                                     
//                                                                            
//   This library is free software; you can redistribute it and/or modify it 
//   under the terms of the GNU Library General Public License as published  
//   by the Free Software Foundation, version 2.                             
//                                                                             
//   This library is distributed in the hope that it will be useful, but       
//   WITHOUT ANY WARRANTY; without even the implied warranty of                
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
//   Library General Public License for more details.                          
//                                                                            
//   You should have received a copy of the GNU Library General Public         
//   License along with this library; if not, write to the Free Software       
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2003/09/19 21:33:57 $
//                                                                            
//=============================================================================

#ifndef OSGGVTRIANGLEADAPTERALIGNED_H
#define OSGGVTRIANGLEADAPTERALIGNED_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVBVolAdapterExt.h"
#include "OSGGVGroup.h"
#include "OSGGVAdapterFactory.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Additional information required by the table-based realignment algorithm.
 */
template <class REAL, int SIZE>
class DataAligned
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
#ifdef GV_PRECOMPUTEDREALIGN_32BIT
   enum { OccTableHighestBit = 31, OccTableBits = 32 };
   typedef u32              OccTableType;
#else
   enum { OccTableHighestBit = 63, OccTableBits = 64 };
   typedef u64              OccTableType;
#endif
   typedef OccTableType     OccTable[4];
   typedef REAL             OuterMost[4] ;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   DataAligned ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */   
   inline PointClass          getCenter () const;
   inline const REAL*         getRotationCenter () const;
   inline const OccTableType* getOccupancy (unsigned i) const;
   inline const REAL*         getOuterMost (unsigned i) const;

   inline REAL*               getCenterInternal ();
   inline void                setCenterInternal (const REAL* value);
   inline OccTable*           getOccupancyInternal ();
   inline OuterMost*          getOuterMostInternal ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   REAL         m_outerMost[2*SIZE][4];
   OccTableType m_occupancy[2*SIZE][4];
   REAL         m_center[SIZE+3];
};

template <class REAL, int SIZE>
DataAligned<REAL,SIZE>::DataAligned ()
{
}
template <class REAL, int SIZE>
inline const REAL*
DataAligned<REAL,SIZE>::getOuterMost (unsigned i) const
{
   return m_outerMost[i];
}
template <class REAL, int SIZE>
inline const DataAligned<REAL,SIZE>::OccTableType*
DataAligned<REAL,SIZE>::getOccupancy (unsigned i) const
{
   return m_occupancy[i];
}
template <class REAL, int SIZE>
inline PointClass
DataAligned<REAL,SIZE>::getCenter () const
{
   return PointClass(m_center[0], m_center[1], m_center[2]);
}
template <class REAL, int SIZE>
inline const REAL*
DataAligned<REAL,SIZE>::getRotationCenter () const
{
   return m_center+3;
}
template <class REAL, int SIZE>
inline REAL*
DataAligned<REAL,SIZE>::getCenterInternal ()
{
   return m_center;
}
template <class REAL, int SIZE>
inline void                
DataAligned<REAL,SIZE>::setCenterInternal (const REAL* value)
{
   for (unsigned i=0; i<SIZE+3; ++i) {
      m_center[i] = value[i];
   }
}
template <class REAL, int SIZE>
inline DataAligned<REAL,SIZE>::OccTable*
DataAligned<REAL,SIZE>::getOccupancyInternal ()
{
   return m_occupancy;
}
template <class REAL, int SIZE>
inline DataAligned<REAL,SIZE>::OuterMost*
DataAligned<REAL,SIZE>::getOuterMostInternal ()
{
   return m_outerMost;
}


/*! \brief Adapter class for triangles with additional data 
    for the table-based realignment algorithm.
 */
template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGTriangleAligned 
: public OpenSGTriangleBase<BasicTraits>, public BVolAdapter<BVOL>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolAdapter<BVOL>                           Inherited;
   typedef OpenSGTriangleAligned<BasicTraits,BVOL>     Self;
   typedef FactoryHeap<Self>                           FactoryType;
   typedef DataAligned<Real,BVOL::Size>                Data;
   enum { OccTableHighestBit = Data::OccTableHighestBit, 
	  OccTableBits       = Data::OccTableBits };
   typedef Data::OccTableType                          OccTableType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   OpenSGTriangleAligned ();
   OpenSGTriangleAligned (const GeomTriangleType& obj);
   OpenSGTriangleAligned (const TransformType&    m2w,
			  const GeomTriangleType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   void  init (const PointClass& p0, const PointClass& p1, const PointClass& p2);
   void  init (const GeomTriangleType& obj);
   void  init (const TransformType&    m2w,
	       const GeomTriangleType& obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline Data&       getData ();
   inline const Data& getData () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface of BVolAdapterBase.                                */
   /*! \{                                                                 */
   virtual inline PointClass                  getCenter ();
   virtual inline const BoundingVolume<Real>& getBoundingVolume () const;
   inline const BVOL&                         getBVol () const;
   virtual void          draw ();
   virtual void          drawWireframe ();
   virtual bool          calcIntersect (Intersection& hit);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */   
   virtual std::ostream& dump (std::ostream& os) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \brief Computation of additional information for a point at distance len
              from the center and direction local.
    */
   void setPoint (unsigned k, Real len, VectorClass local);
   /*---------------------------------------------------------------------*/

   BVOL m_bvol;
   Data m_data;
};

template <class BasicTraits, class BVOL>
inline OpenSGTriangleAligned<BasicTraits,BVOL>::Data&
OpenSGTriangleAligned<BasicTraits,BVOL>::getData () 
{
   return m_data;
}
template <class BasicTraits, class BVOL>
inline const OpenSGTriangleAligned<BasicTraits,BVOL>::Data&
OpenSGTriangleAligned<BasicTraits,BVOL>::getData () const
{
   return m_data;
}
template <class BasicTraits, class BVOL>
inline PointClass OpenSGTriangleAligned<BasicTraits,BVOL>::getCenter()
{
   return getData().getCenter();   
}
template <class BasicTraits, class BVOL>
inline const BVOL& OpenSGTriangleAligned<BasicTraits,BVOL>::getBVol () const
{
   return m_bvol;
}
template <class BasicTraits, class BVOL>
inline const BoundingVolume<Real>& 
OpenSGTriangleAligned<BasicTraits,BVOL>::getBoundingVolume () const
{
   return m_bvol;
}


/*! \brief Group class for table-based realignment algorithm.
 */
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING BVolGroupAligned 
: public BVolGroup<BVOL>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolGroup<BVOL>                             Inherited;
   typedef BVolGroupAligned<BVOL>                      Self;
   typedef FactoryHeap<Self>                           FactoryType;
   typedef DataAligned<Real,BVOL::Size>                Data;
   enum { OccTableHighestBit = Data::OccTableHighestBit, 
	  OccTableBits       = Data::OccTableBits };
   typedef typename Data::OccTableType                 OccTableType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline BVolGroupAligned ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline Data& getData ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   void init (const BVolGroupAligned<BVOL>& src);
   void init (unsigned first,
	      unsigned last,
	      const std::vector< std::vector<unsigned> >& index,
	      const std::vector<Adapter*>&                nodes);
   void updateBoundingVolume (unsigned first,
			      unsigned last,
			      const std::vector< std::vector<unsigned> >& index,
			      const std::vector<Adapter*>&                nodes);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Interface BVolAdapterBase.                                   */
   /*! \{                                                                 */
   virtual inline PointClass           getCenter ();
   virtual const BoundingVolume<Real>& getBoundingVolume () const;

   virtual void              draw ();
   virtual void              drawWireframe ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \brief Computation of additional information for a point at distance len
              from the center and direction local.
    */
   void setPoint (unsigned k, Real len, VectorClass local);
   /*---------------------------------------------------------------------*/
   /*! Create TEXTURE_1D for occupancy table mask. This function is used
       as a helper to visualize occupancy tables.                         */
   static u8* texTable (const OccTableType& mask);
   /*---------------------------------------------------------------------*/

   Data m_data;
};

template <class BVOL>
inline BVolGroupAligned<BVOL>::BVolGroupAligned ()
{
}
template <class BVOL>
inline BVolGroupAligned<BVOL>::Data&
BVolGroupAligned<BVOL>::getData ()
{
   return m_data;
}
template <class BVOL>
inline PointClass BVolGroupAligned<BVOL>::getCenter()
{
   return getData().getCenter();
}

END_GENVIS_NAMESPACE
#endif






