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
//   $Date: 2004/03/12 13:23:23 $
//                                                                            
//=============================================================================

#ifndef OSGGVKDOP_H
#define OSGGVKDOP_H

#include <string>
#include <iostream>
#include "OSGGVBase.h"
#include "OSGGVBoundingVolume.h"
#include "OSGGVTimer.h"

BEGIN_GENVIS_NAMESPACE

template<class REAL, int SIZE> class KDopGeometry;

/*! \brief k-DOP bounding volume, defined by k real intervals.
 */
template <class REAL, int SIZE>
class OSG_GENVISLIB_DLLMAPPING KDop : public BoundingVolume<REAL>
{
   friend class KDopGeometry<REAL,SIZE>;
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef KDop<REAL,SIZE>                           Self;
   typedef BoundingVolume<REAL>                      Inherited;
   enum                                            { Size = SIZE };
   typedef KDop<REAL,SIZE>                           BVol;
   typedef KDopGeometry<REAL,SIZE>                   BVolGeometry;
#ifdef GV_PRECOMPUTEDREALIGN_32BIT
   enum { OccTable2HighestBit = 30, OccTableHighestBit = 31, OccTableBits = 32 };
   typedef u32                                       OccTableType;
#else
   enum { OccTable2HighestBit = 62, OccTableHighestBit = 63, OccTableBits = 64 };
   typedef u64                                       OccTableType;
#endif
   typedef const REAL                                DopTableType[OccTableBits];
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   KDop ();
   KDop (const REAL* minVector, 
	 const REAL* maxVector);
   explicit KDop (const BoundingVolume<REAL>& bvol); 
   KDop (const PointClass& center, 
	 REAL hx, REAL hy, REAL hz);
   const KDop<REAL,SIZE>& operator= (const KDop<REAL,SIZE>& dop);
   KDop (const KDop<REAL,SIZE>& dop);
   virtual ~KDop ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */
   void init (const BoundingVolume<REAL>& bvol); 
   void init (const PointClass& center, 
	      REAL hx, REAL hy, REAL hz);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Update associated members.                                   */
   /*! \{                                                                 */
   /*! Signal update of associated members.                               */
   virtual void update ();
   /*! Update boundary representation if necessary.                       */
   void         updateGeometry () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   /*! Center point.                                                      */
   virtual PointClass         getCenter () const;
   /*! Center point.                                                      */
   virtual inline void        getCenter (PointClass& center) const;
   /*! Boundary representation (explicit representation) of KDop.         */
   BVolGeometry&              getGeometry () const;

   inline VectorClass         getNormal   (u32 i) const;
   inline VectorClass         planeNormal (i32 i) const;
   inline REAL                getScalar   (u32 i) const;
   inline REAL                planeScalar (i32 i) const;

   /*! Array of KDop scalars to directions Size,..,2*Size-1.              */
   virtual inline REAL*       minVector ();
   /*! Array of KDop scalars to directions 0,..,Size-1.                   */
   virtual inline REAL*       maxVector ();
   /*! Array of KDop scalars to directions Size,..,2*Size-1.              */
   virtual inline const REAL* minVector () const;
   /*! Array of KDop scalars to directions 0,..,Size-1.                   */
   virtual inline const REAL* maxVector () const;
   /*! KDop scalar to direction i, i in 0,..,2*Size-1                     */
   inline REAL                getVector (u32 i) const;
   /*! KDop scalar to slab i, i in 0,..,Size-1.                           */
   inline REAL                getMinVector (u32 i) const;
   /*! KDop scalar to slab i, i in 0,..,Size-1.                           */
   inline REAL                getMaxVector (u32 i) const;
   /*! Slab thickness for slab i, i in 0,..,Size-1.                       */
   virtual inline REAL        difference (u32 k) const;
   /*! Slab middle for slab i, i in 0,..,Size-1.                          */
   virtual inline REAL        average    (u32 k) const;
   /*! Two times the result of difference(k).                             */
   virtual inline REAL        addition   (u32 k) const;
   /*! Array of KDop directions.                                          */
   inline const VectorClass*  direction () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Geometric size.                                              */
   /*! \{                                                                 */
   virtual REAL        getMinmaxFraction () const;
   virtual inline REAL getDiameter  () const;
   virtual REAL        getSurfaceArea () const;
   virtual REAL        getVolume      () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Intersection tests.                                          */
   /*! \{                                                                 */
   virtual bool checkIntersect (const PointClass&  origin, 
				const VectorClass& dir,
				const REAL& dist) const;
   /*! Calculate intersection with ray defined by origin, direction.
       Returns true iff ray intersects kdop at distance dist.
   */
   virtual bool calcIntersect (const PointClass&  origin, 
			       const VectorClass& dir,
			       REAL& dist) const;
   /*! Test for intersection with point. */
   virtual bool checkIntersect (const PointClass& point) const;
   /*! Test for intersection with another bounding volume. */
   virtual bool checkIntersect (const BoundingVolume<REAL>& box) const;
   /*! Test for intersection with a triangle (p1, p2, p3). */
   virtual bool checkIntersect (const PointClass& p1,
				const PointClass& p2,
				const PointClass& p3) const;
   /*! Test for intersection with another kdop. */
   bool checkIntersect (const KDop<REAL,SIZE>& dop2) const;
   /*! Test for intersection with a translated kdop. */
   bool checkIntersect (const KDop<REAL,SIZE>& dop2,
			REAL*                  offset) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Unification.                                                 */
   /*! \{                                                                 */
   void         unify (const KDop<REAL,SIZE>& dop);
   void         unify (const KDop<REAL,SIZE>& dop,
		       REAL*                  offset);
   virtual void unify (float size);
   virtual void unify (const BoundingVolume<REAL>& box);
   virtual void unify (const PointClass& point);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Draw functions.                                              */
   /*! \{                                                                 */
   virtual void draw ()          const;
   virtual void drawWireframe () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual std::ostream& dumpInternal (std::ostream& os) const;
   static void           statistics   (std::string& str);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Set of directions.                                           */
   /*! \{                                                                 */
   /*! Create KDop directions. According to the template argument SIZE a
       reasonable set of KDop directions is calculated. For example SIZE=3,
       SIZE=7, SIZE=9, SIZE=13 the directions proposed by Klosowski is 
       chosen.                                                            */
   static bool                      createDirections ();
   /*! Array of KDop directions.                                          */
   static const VectorClass* getDirection ();
   /*! Set array of KDop directions.                                      */
   static void               setDirection (VectorClass* dir);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Specialities.                                                */
   /*! \{                                                                 */
   /*! KDop with all scalars equal to one.                                */
   static const BVol&               unitDop ();
   /*! Radius of a face of the unit KDop (seen form the center point).    */
   static REAL                      unitDopRadius ();
   /*! Aperture angle (cosine) of a face of the unit KDop 
       (seen form the center point).                                      */
   static REAL                      unitDopAngle       ();

   /*! Create monotonously decreasing sequence 1,..,unitDopAngle() 
       of angles (cosine). Length is OccTableBits.                        */
   static DopTableType*             unitDopTable       ();
   /*! */
   static const REAL*               unitDopAngleTable  ();
   /*! */
   static REAL                      unitDopAngleTable (const OccTableType& mask, 
						       i32                 p,
						       REAL                out);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   static VectorClass s_direction[2*SIZE];
   static bool        s_init;

   REAL               m_min[SIZE];
   REAL               m_max[SIZE];
   BVolGeometry*      m_geom;
};

typedef KDop<Real,7>         K14Dop;
typedef KDop<Real,9>         K18Dop;
typedef KDop<Real,13>        K26Dop;
// platonic solids
// * cube
typedef KDop<Real,3>         AABox;
typedef KDop<Real,3>         K6Dop;
// * icosahedron
typedef KDop<Real,10>        Icosahedron;
typedef KDop<Real,10>        K20Dop;
// * dodecahedron
typedef KDop<Real,6>         Dodecahedron;
typedef KDop<Real,6>         K12Dop;

template <class REAL, int SIZE>
inline REAL* KDop<REAL,SIZE>::minVector ()
{
   return m_min;
}
template <class REAL, int SIZE>
inline REAL* KDop<REAL,SIZE>::maxVector ()
{
   return m_max;
}
template <class REAL, int SIZE>
inline const REAL* KDop<REAL,SIZE>::minVector () const
{
   return m_min;
}
template <class REAL, int SIZE>
inline const REAL* KDop<REAL,SIZE>::maxVector () const
{
   return m_max;
}
template <class REAL, int SIZE>
inline REAL KDop<REAL,SIZE>::getVector (u32 i) const
{
   assert(i < 2*SIZE);
   if (i < SIZE) {
      return m_max[i];
   } else {
      return m_min[i-SIZE];
   }
}
template <class REAL, int SIZE>
inline REAL KDop<REAL,SIZE>::getMinVector (u32 i) const
{
   assert(i < 2*SIZE);
   if (i < SIZE) {
      return m_min[i];
   } else {
      return -m_max[i-SIZE];
   }
}
template <class REAL, int SIZE>
inline REAL KDop<REAL,SIZE>::getMaxVector (u32 i) const
{
   assert(i < 2*SIZE);
   if (i < SIZE) {
      return m_max[i];
   } else {
      return -m_min[i-SIZE];
   }
}

template <class REAL, int SIZE>
inline VectorClass KDop<REAL,SIZE>::planeNormal (i32 i) const
{
   assert(stdAbs(i) <= SIZE);
   if (i > 0) {
      return getDirection()[i-1];
   } else {
      return getDirection()[SIZE-i-1];
   }
}
template <class REAL, int SIZE>
inline VectorClass KDop<REAL,SIZE>::getNormal   (u32 i) const
{
   assert(i < 2*SIZE);
   return getDirection()[i];
}
template <class REAL, int SIZE>
inline REAL              KDop<REAL,SIZE>::planeScalar (i32 i) const
{
   assert(stdAbs(i) <= SIZE);
   if (i > 0) {
      return m_max[i-1];
   } else {
      return -m_min[-i-1];
   }
}
template <class REAL, int SIZE>
inline REAL              KDop<REAL,SIZE>::getScalar (u32 i) const
{
   assert(i < 2*SIZE);
   if (i < SIZE) {
      return m_max[i];
   } else {
      return -m_min[i-SIZE];
   }
}

template <class REAL, int SIZE>
inline REAL KDop<REAL,SIZE>::difference (u32 k) const
{
   return m_max[k]-m_min[k];
}
template <class REAL, int SIZE>
inline REAL KDop<REAL,SIZE>::addition (u32 k) const
{
   return m_max[k]+m_min[k];
}
template <class REAL, int SIZE>
inline REAL KDop<REAL,SIZE>::average (u32 k) const
{
   return 0.5f*(m_max[k]+m_min[k]);
}
template <class REAL, int SIZE>
inline REAL KDop<REAL,SIZE>::getDiameter () const
{
   REAL d0 = difference(0);
   REAL d1 = difference(1);
   REAL d2 = difference(2);
   return sqrt(d0*d0+d1*d1+d2*d2);
}

template <class REAL, int SIZE>
inline void KDop<REAL,SIZE>::getCenter (PointClass& center) const
{
   center = getCenter();
}

template <class REAL, int SIZE>
inline const VectorClass* KDop<REAL,SIZE>::direction () const
{ 
   return getDirection();
}

END_GENVIS_NAMESPACE
#endif



