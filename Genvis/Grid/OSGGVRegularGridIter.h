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
//   $Date: 2004/12/20 15:55:46 $
//                                                                            
//=============================================================================

#ifndef OSGGVREGULARGRIDITER_H
#define OSGGVREGULARGRIDITER_H

#include "OSGGVBase.h"
#include "OSGGVRay.h"
#include "OSGGVRegularGridBase.h"

BEGIN_GENVIS_NAMESPACE

/** Class RegularGridIter implements the traversal of a ray through
    a voxel space */    
class OSG_GENVISLIB_DLLMAPPING RegularGridIter
{
public:
   /*---------------------------------------------------------------------*/
   /** \name Constructors.                                                */
   /** \{                                                                 */
   /** Constructor.
       @param vs(in)   Voxel space to traverse with a ray
       @param ray(in)  Ray
   */
   RegularGridIter (RegularGridBase& vs, const Ray& ray);

   /** */
   virtual ~RegularGridIter ();
   /** \}                                                                 */
   /*---------------------------------------------------------------------*/
   /** \name Traversal.                                                   */
   /** \{                                                                 */   
   /** Goto next voxel in traversal direction.
       Return false if no voxels are left along ray. */
   bool operator() ();
   /** \}                                                                 */
   /*---------------------------------------------------------------------*/
   /** \name Access. The following member functions are only defined if
       the previous call of operator() returned true. */
   /** \{                                                                 */   
   /** Current voxel indices in voxel space. */
   inline const VectorClass3u& getIndexDim () const;
   /** px() + p.y()*vs.nx() + pz()*vs.nx()*vs.ny() */
   inline u64          getIndex () const;
   /** Distance from point to first intersection
       of ray and current voxel in direction dir.
       If point is inside the voxel then the returned value is 0. */
   inline Real         getInDist () const;
   /** Distance from point to last intersection 
       of ray and current voxel in direction dir. */
   inline Real         getOutDist() const;
   /** \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   /** Current voxel coordinates */
   VectorClass3u m_v;
   /** */
   u64 m_arrayPos;
   /** values to intersection plane */
   VectorClass m_ta;
   /** delta values of parallel voxel-planes */
   VectorClass m_dt;
   /** voxel stepping */
   VectorClass3i m_vStep;
   /** voxel array stepping */
   VectorClass3i m_arrayStep;
   /** voxel system exit coordinates */
   VectorClass3i m_endPos;
   /** inDist() */
   Real m_voxHitDist;
   /** current voxel first ? */
   bool m_first;
   /** no more voxels left to test */
   bool m_finished;
};

inline const VectorClass3u& RegularGridIter::getIndexDim () const 
{ 
   return m_v; 
}
inline u64 RegularGridIter::getIndex () const 
{ 
   return m_arrayPos; 
}
inline Real RegularGridIter::getInDist () const 
{ 
   return m_voxHitDist; 
}
inline Real RegularGridIter::getOutDist() const
{ 
   return stdMin(m_ta[0], stdMin(m_ta[1], m_ta[2])); 
}

END_GENVIS_NAMESPACE
#endif
