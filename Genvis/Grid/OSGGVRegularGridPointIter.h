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

#ifndef OSGGVREGULARGRIDPOINTITER_H
#define OSGGVREGULARGRIDPOINTITER_H

#include "OSGGVBase.h"
#include "OSGGVRay.h"
#include "OSGGVRegularGridBase.h"

BEGIN_GENVIS_NAMESPACE

/** Class RegularGridPointIter implements the search for nearest adapters. */    
class OSG_GENVISLIB_DLLMAPPING RegularGridPointIter
{
public:
   /*---------------------------------------------------------------------*/
   /** \name Constructors.                                                */
   /** \{                                                                 */
   /** Constructor.
       @param vs(in)    Voxel space to search for nearest adapters.
       @param point(in) Query point
   */
   RegularGridPointIter (RegularGridBase& vs, PointClass point);
   /** \}                                                                 */
   /*---------------------------------------------------------------------*/
   /** \name Traversal.                                                   */
   /** \{                                                                 */   
   /** Goto next voxel in search order.
       Return number of voxel rings to be processed. */
   i32 operator() ();
   /** \}                                                                 */
   /*---------------------------------------------------------------------*/
   /** \name Access. The following member functions are only defined if
       the previous call of operator() returned true. */
   /** \{                                                                 */   
   /** Current voxel indices in voxel space. */
   inline const VectorClass3u& getIndexDim () const;
   /** px() + p.y()*vs.nx() + pz()*vs.nx()*vs.ny() */
   inline u64                  getIndex () const;
   /** \}                                                                 */
   /*---------------------------------------------------------------------*/
   inline RegularGridBase&     getGrid () const;

private:
   RegularGridBase* m_grid;
   VectorClass3u m_coord;
   u64           m_index;
   i32 m_d;
   i32 m_dmax;
   VectorClass3i m_start;
   VectorClass3i m_diff;
   bool m_first;
};

inline const VectorClass3u& RegularGridPointIter::getIndexDim () const 
{ 
   return m_coord; 
}
inline u64 RegularGridPointIter::getIndex () const 
{ 
   return m_index; 
}

inline RegularGridBase& RegularGridPointIter::getGrid () const
{
   assert(m_grid != NULL);
   return *m_grid;
}

END_GENVIS_NAMESPACE
#endif
