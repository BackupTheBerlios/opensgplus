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
//   $Date: 2003/09/19 21:48:57 $
//                                                                            
//=============================================================================

#ifndef OSGGVKDOPGEOMETRY_H
#define OSGGVKDOPGEOMETRY_H

#include <vector>
#include "OSGGVBase.h"
#include "OSGGVReal.h"
#include "OSGGVPolygon.h"
#include "OSGGVKDop.h"

BEGIN_GENVIS_NAMESPACE
template<class REAL, int SIZE> class KDop;

/*! \brief Boundary representation of a k-DOP bounding volume.
 */
template <class REAL, int SIZE>
class OSG_GENVISLIB_DLLMAPPING KDopGeometry
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef KDopGeometry<REAL,SIZE>    Self;
   enum                               { Size = SIZE };
   typedef KDop<REAL,SIZE>            BVol;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline KDopGeometry (const BVol& bvol);
   inline KDopGeometry (const Self& src);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */
   inline Polygon3SetIndexed&       getPolygonSet ();
   inline const Polygon3SetIndexed& getPolygonSet () const;
   inline const BVol&               getBVol () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Draw functions.                                              */
   /*! \{                                                                 */
   void  draw          () const;
   void  drawWireframe () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Geometric size.                                              */
   /*! \{                                                                 */
   REAL getSurfaceArea () const;
   REAL getVolume      () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Local frame.                                                 */
   /*! \{                                                                 */
   static inline VectorClass* getFrameX ();
   static inline VectorClass* getFrameY ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   /*---------------------------------------------------------------------*/
   /*! \name Computation.                                                 */
   /*! \{                                                                 */
   /*! Recompute boundary representation of bvol.                         */
   void              calcGeometry ();
   /*! Create local frame for each KDop direction. Called once during 
       static intialization.                                              */
   static bool       createFrames ();
   /*! Create boundary representation geom with the single KDop direction 0.
       This method is used for non-orthonormal directions.                */
   static void       createSlab  (const BVol& bvol,
				  Polygon3SetIndexed&    geom);
   /*! Create boundary representation geom with the KDop directions 0, 1, 2.
       This method is used for orthonormal directions, in which case a 
       box shape results.                                                 */
   static void       createBox   (const BVol& bvol,
				  Polygon3SetIndexed&    geom);
   /*! Cut boundary representation geom using the KDop direction i.       */
   static void       cutHalfspace(unsigned i,
				  const BVol& bvol,
				  Polygon3SetIndexed&    geom);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   static VectorClass s_x[SIZE+SIZE];
   static VectorClass s_y[SIZE+SIZE];
   static bool        s_init;
   const BVol*        m_bvol;
   Polygon3SetIndexed m_geom;
};

template <class REAL, int SIZE>
inline Polygon3SetIndexed& KDopGeometry<REAL,SIZE>::getPolygonSet ()
{
   return m_geom;
}
template <class REAL, int SIZE>
inline const Polygon3SetIndexed& KDopGeometry<REAL,SIZE>::getPolygonSet () const
{
   return m_geom;
}
template <class REAL, int SIZE>
inline const KDopGeometry<REAL,SIZE>::BVol&
KDopGeometry<REAL,SIZE>::getBVol () const
{
   assert(m_bvol != NULL);
   return *m_bvol;
}
template <class REAL, int SIZE>
inline VectorClass* KDopGeometry<REAL,SIZE>::getFrameX ()
{
   return s_x;
}
template <class REAL, int SIZE>
inline VectorClass* KDopGeometry<REAL,SIZE>::getFrameY ()
{
   return s_y;
}

template <class REAL, int SIZE>
inline KDopGeometry<REAL,SIZE>::KDopGeometry (const BVol& bvol)
  : m_bvol(&bvol)
{
   calcGeometry ();
}
template <class REAL, int SIZE>
inline KDopGeometry<REAL,SIZE>::KDopGeometry (const Self& src)
  : m_bvol(src.m_bvol), m_geom(src.m_geom)
{
}

END_GENVIS_NAMESPACE
#endif
