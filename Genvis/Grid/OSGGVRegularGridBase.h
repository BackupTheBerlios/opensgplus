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
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//=============================================================================

#ifndef OSGGVREGULARGRIDBASE_H
#define OSGGVREGULARGRIDBASE_H

#include "OSGGVBase.h"
#include "OSGGVKDop.h"

BEGIN_GENVIS_NAMESPACE

/*! Class RegularGridBase implements an uniformly sudivided space. */
class OSG_GENVISLIB_DLLMAPPING RegularGridBase : public K6Dop
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   /*! Number of voxel created during setup depends on
       \begin{itemize}
       \item VoxelsPerUnit   Number of voxels per model unit.
       \item MaxVoxels       Maximum number of voxels allowed.
       \item MaxVoxelsPerDim Maximum number of voxels per dimension.
       \item MinVoxelsPerDim Minimum number of voxels per dimension. 
       \end{itemize} 
   */
   enum InitMode { 
      VoxelsPerUnit, MaxVoxelsPerDim, MinVoxelsPerDim, MaxVoxels
   };
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   /*! Default constructor. */
   RegularGridBase ();
   /*! Constructor.
       @param cen(in) Center point of axes-alignde box.
       @param hlx(in) Half length in x.
       @param hly(in) Half length in y.
       @param hlz(in) Half length in z.
       @param vnx(in) Number of voxels in x.
       @param vny(in) Number of voxels in y.
       @param vnz(in) Number of voxels in z. */
   inline RegularGridBase (const PointClass& cen, 
		      Real hlx, Real hly, Real hlz,
		      int vnx, int vny, int vnz);
   /*! Destructor. */
   virtual ~RegularGridBase ();
   /*! Initialization. */
   void init (const PointClass& center, 
	      Real hlx, Real hly, Real hlz,
              i32 vnx, i32 vny, i32 vnz);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */   
   /*! Number of voxels in each dimension. */
   inline const VectorClass3i& getNumVoxelsDim () const;
   /*! Total number of voxels. */ 
   inline i64           getNumVoxels   () const;
   /*! Number of voxels in a z-slice. */
   inline i32           getNumVoxelsXY () const;
   /*! Strides for addressing neighbour voxels. */
   inline const VectorClass3i& getStrides () const;
   /*! Lengths of voxel edges parallel to the axes. */
   inline const VectorClass& getLength() const;
   /*! Inverse lengths of voxel edges parallel to the axes. */
   inline const VectorClass& getInvLength() const;
   /*! Halflength of voxel edge parallel to the x axis. */
   inline const VectorClass& getHalfLength() const;
   /*! Center of reference voxel (0, 0, 0). */
   inline const PointClass&  getRefCenter() const;
   /*! Center of voxel (x, y, z). */
   inline PointClass         getRefCenter (i32 x, i32 y, i32 z) const;
   /*! Bounding box of voxel (x, y, z). */
   inline void               getVoxel (K6Dop& box, i32 x, i32 y, i32 z) const;
   /*! Transform euclidean coordinates to voxel coordinates. */
   inline void               toVoxel     (PointClass& p) const;
   /*! Transform voxel coordinates to euclidean coordinates. */
   inline void               toEuclidean (PointClass& p) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*! Number of voxels in each dimension. */
   VectorClass3i m_num;
   /*! Total number of voxels. */
   i64    m_numVoxels;
   /*! Stride values. */
   VectorClass3i m_strides;
   // some frequently used constants:
   VectorClass m_vlength;
   VectorClass m_inv_vlength;
   VectorClass m_vh;
   PointClass  m_vcen;
};

inline RegularGridBase::RegularGridBase (const PointClass& cen, 
			       Real hlx, Real hly, Real hlz,
			       int vnx, int vny, int vnz)
  : K6Dop()
{
   init(cen, hlx,hly,hlz, vnx,vny,vnz);
}

inline const VectorClass3i& RegularGridBase::getNumVoxelsDim () const 
{ 
   return m_num; 
}  

inline i64 RegularGridBase::getNumVoxels () const 
{ 
   return m_numVoxels; 
}
inline i32 RegularGridBase::getNumVoxelsXY () const 
{ 
   return m_strides[2]; 
}
inline const VectorClass3i& RegularGridBase::getStrides () const
{
   return m_strides;
}
inline const VectorClass& RegularGridBase::getLength() const 
{ 
   return m_vlength; 
}
inline const VectorClass& RegularGridBase::getInvLength() const 
{ 
   return m_inv_vlength; 
}
inline const VectorClass& RegularGridBase::getHalfLength() const 
{ 
   return m_vh; 
}
inline const PointClass& RegularGridBase::getRefCenter() const 
{ 
   return m_vcen; 
}
inline PointClass RegularGridBase::getRefCenter (i32 x, i32 y, i32 z) const 
{ 
   VectorClass translate(x, y, z);
   translate[0] *= getLength()[0];
   translate[1] *= getLength()[1];
   translate[2] *= getLength()[2];
   return getRefCenter()+translate;
}

inline void RegularGridBase::getVoxel (K6Dop& box, i32 x, i32 y, i32 z) const
{
   box.init(getRefCenter(x, y, z), getHalfLength()[0], getHalfLength()[1], getHalfLength()[2]);
}

inline void RegularGridBase::toVoxel (PointClass& p) const
{
   p[0] -= minVector()[0];
   p[1] -= minVector()[1];
   p[2] -= minVector()[2];
   p[0] *= getInvLength()[0];
   p[1] *= getInvLength()[1];
   p[2] *= getInvLength()[2];
}
inline void RegularGridBase::toEuclidean (PointClass& p) const
{
   p[0] *= getLength()[0];
   p[1] *= getLength()[1];
   p[2] *= getLength()[2];
   p[0] += minVector()[0];
   p[1] += minVector()[1];
   p[2] += minVector()[2];
}

END_GENVIS_NAMESPACE
#endif
